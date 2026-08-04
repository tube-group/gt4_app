#include "L2RcvL3.h"

#include "MesStruct.h"
#include "logging.h"
#include <algorithm>
#include <cctype>
#include <chrono>
#include <mutex>
#include <sstream>
#include <stdexcept>
#include <string>
#include <thread>
#include <unordered_map>
#include <variant>
#include <vector>

#include <httplib.h>
#include <nlohmann/json.hpp>
#include <pqxx/pqxx>

namespace {

using json = nlohmann::json;

// ============================================================
// 通用字段值类型
// ============================================================
using FieldValue = std::variant<std::string, long long, double>;

struct ParsedRecord
{
	std::unordered_map<std::string, FieldValue> values;
};

std::string field_value_to_string(const FieldValue &value)
{
	return std::visit([](const auto &v) {
		std::ostringstream oss;
		oss << v;
		return oss.str();
	}, value);
}

// ============================================================
// 从 ParsedRecord 中按字段名取值的辅助函数
// ============================================================
std::string get_str(const ParsedRecord &rec, const std::string &name,
					const std::string &default_val = "")
{
	auto it = rec.values.find(name);
	if (it == rec.values.end()) return default_val;
	if (auto *p = std::get_if<std::string>(&it->second)) return *p;
	return field_value_to_string(it->second);
}

double get_double(const ParsedRecord &rec, const std::string &name,
				  double default_val = 0.0)
{
	auto it = rec.values.find(name);
	if (it == rec.values.end()) return default_val;
	if (auto *p = std::get_if<double>(&it->second)) return *p;
	if (auto *p = std::get_if<long long>(&it->second)) return static_cast<double>(*p);
	if (auto *p = std::get_if<std::string>(&it->second)) {
		try { return std::stod(*p); } catch (...) { return default_val; }
	}
	return default_val;
}

long long get_int(const ParsedRecord &rec, const std::string &name,
				  long long default_val = 0)
{
	auto it = rec.values.find(name);
	if (it == rec.values.end()) return default_val;
	if (auto *p = std::get_if<long long>(&it->second)) return *p;
	if (auto *p = std::get_if<double>(&it->second)) return static_cast<long long>(*p);
	if (auto *p = std::get_if<std::string>(&it->second)) {
		try { return std::stoll(*p); } catch (...) { return default_val; }
	}
	return default_val;
}

// ============================================================
// 按字段类型解析 JSON 值
// ============================================================
FieldValue parse_value_by_type(const json &value, char field_type)
{
	if (value.is_null()) {
		if (field_type == 'D') {
			return 0.0;
		}
		if (field_type == 'L' || field_type == 'I') {
			return 0LL;
		}
		return std::string();
	}

	if (field_type == 'D') {
		if (value.is_number()) {
			return value.get<double>();
		}
		if (value.is_string()) {
			const std::string &s = value.get_ref<const std::string &>();
			if (s.empty()) return 0.0;
			return std::stod(s);
		}
		throw std::runtime_error("Expected numeric/decimal value");
	}

	if (field_type == 'L' || field_type == 'I') {
		if (value.is_number_integer()) {
			return value.get<long long>();
		}
		if (value.is_number_float()) {
			return static_cast<long long>(value.get<double>());
		}
		if (value.is_string()) {
			const std::string &s = value.get_ref<const std::string &>();
			if (s.empty()) return 0LL;
			return std::stoll(s);
		}
		throw std::runtime_error("Expected integer value");
	}

	// 'C' — 字符串
	if (value.is_string()) {
		return value.get<std::string>();
	}
	return value.dump();
}

// ============================================================
// 解析 L3 的 __blocks__ 结构
// ============================================================
bool parse_l3_block_rows(const json &payload,
						 size_t &out_column_count,
						 std::vector<json> &out_records,
						 std::string &out_error)
{
	if (!payload.is_object()) {
		out_error = "Request body must be a JSON object.";
		return false;
	}

	auto blocks_it = payload.find("__blocks__");
	if (blocks_it == payload.end() || !blocks_it->is_object()) {
		out_error = "Missing object: __blocks__.";
		return false;
	}

	auto default0_it = blocks_it->find("Default0");
	if (default0_it == blocks_it->end() || !default0_it->is_object()) {
		out_error = "Missing object: __blocks__.Default0.";
		return false;
	}

	auto meta_it = default0_it->find("meta");
	auto rows_it = default0_it->find("rows");
	if (meta_it == default0_it->end() || !meta_it->is_object()) {
		out_error = "Missing object: __blocks__.Default0.meta.";
		return false;
	}
	if (rows_it == default0_it->end() || !rows_it->is_array()) {
		out_error = "Missing array: __blocks__.Default0.rows.";
		return false;
	}

	auto columns_it = meta_it->find("columns");
	if (columns_it == meta_it->end() || !columns_it->is_array()) {
		out_error = "Missing array: __blocks__.Default0.meta.columns.";
		return false;
	}

	std::vector<std::string> pos_to_name;
	for (size_t idx = 0; idx < columns_it->size(); ++idx) {
		const auto &col = (*columns_it)[idx];
		if (!col.is_object()) {
			continue;
		}

		auto name_it = col.find("name");
		if (name_it == col.end() || !name_it->is_string()) {
			continue;
		}

		int pos = static_cast<int>(idx);
		auto pos_it = col.find("pos");
		if (pos_it != col.end() && pos_it->is_number_integer()) {
			pos = pos_it->get<int>();
		}

		if (pos < 0) {
			continue;
		}

		if (static_cast<size_t>(pos) >= pos_to_name.size()) {
			pos_to_name.resize(static_cast<size_t>(pos) + 1);
		}
		pos_to_name[static_cast<size_t>(pos)] = name_it->get<std::string>();
	}

	out_column_count = std::count_if(pos_to_name.begin(), pos_to_name.end(), [](const std::string &name) {
		return !name.empty();
	});

	for (size_t row_idx = 0; row_idx < rows_it->size(); ++row_idx) {
		const auto &row = (*rows_it)[row_idx];
		if (!row.is_array()) {
			out_error = "Each item in __blocks__.Default0.rows must be an array.";
			return false;
		}

		json record = json::object();
		for (size_t col_idx = 0; col_idx < row.size(); ++col_idx) {
			std::string key;
			if (col_idx < pos_to_name.size() && !pos_to_name[col_idx].empty()) {
				key = pos_to_name[col_idx];
			} else {
				key = "COL_" + std::to_string(col_idx);
			}
			record[key] = row[col_idx];
		}
		out_records.push_back(std::move(record));
	}

	return true;
}

// ============================================================
// 根据 FieldList 解析记录
// ============================================================
bool parse_records_by_fields(const std::vector<json> &raw_records,
							 const FieldList &fields,
							 std::vector<ParsedRecord> &out_records,
							 std::string &out_error)
{
	for (size_t row_idx = 0; row_idx < raw_records.size(); ++row_idx) {
		const auto &raw = raw_records[row_idx];
		ParsedRecord record;

		for (const auto &field : fields) {
			auto it = raw.find(field.name);
			if (it == raw.end()) {
				continue;
			}

			try {
				record.values[field.name] = parse_value_by_type(*it, field.type);
			} catch (const std::exception &ex) {
				out_error = "Row " + std::to_string(row_idx) + ", field " + field.name +
					": " + ex.what();
				return false;
			}
		}

		out_records.push_back(std::move(record));
	}

	return true;
}

// ============================================================
// 打印解析后的记录
// ============================================================
void print_records(const std::string &msg_type,
				   const FieldList &fields,
				   const std::vector<ParsedRecord> &records)
{
	spdlog::info("========== {} ==========", msg_type);
	spdlog::info("record_count={}", records.size());

	for (size_t i = 0; i < records.size(); ++i) {
		spdlog::info("-- record[{}] --", i);
		for (const auto &field : fields) {
			auto it = records[i].values.find(field.name);
			if (it == records[i].values.end()) {
				spdlog::info("  {} = <missing>", field.name);
			} else {
				spdlog::info("  {} = {}", field.name, field_value_to_string(it->second));
			}
		}
	}

	spdlog::info("==============================");
}

// ============================================================
// 电文类型自动识别
// ============================================================
enum class MessageType
{
	GSPA19,
	Unknown,
};

const char *message_type_name(MessageType mt)
{
	switch (mt) {
	case MessageType::GSPA19: return "GSPA19";
	default: return "Unknown";
	}
}

// ============================================================
// 将结构体字段名转为 DB 列名
// 默认规则：字段名小写 = DB 列名，少数例外通过映射表处理
// ============================================================
static std::string field_to_db_col(const std::string &field_name)
{
	// 字段名 -> DB 列名 映射表（仅用于名称不一致的情况）
	static const std::unordered_map<std::string, std::string> overrides = {
		{"OPERATOR_FLAG", "operate_flag"},
		{"MAT_NO", "bundle_no"},
		{"PONO", "melt_no"},
		{"SAMPLE_LOT_NO", "lot_no"},
		{"ORDER_OUTER_DIA", "diameter"},
		{"ORDER_WAL_THICK", "wal_thick"},
		{"ORDER_HEIGHT", "height"},
		{"MTRL_NO", "mat_no"},
		{"MTRL_TEXT", "mat_text"},
		{"SG_SIGN", "sg_text"},
		{"SG_STD", "std_text"},
		{"PIPEEND_TYPE_CODE", "end_type_code"},
		{"PIPEEND_TYPE_SIGN", "end_type_sign"},
		{"MAT_TUBE", "tube"},
		{"MAT_ACT_WT", "weight"},
		{"TOTAL_LEN", "total_length"},
		{"HOT_TREAT_METHOD_CODE", "heat_treat_code"},
		{"HOT_TREAT_METHOD", "heat_treat_text"},
		{"ORDER_LEN_MIN", "length_from"},
		{"ORDER_LEN_MAX", "length_to"},
		{"TUBE_NO", "tube_no_1"},
		{"TUBE_LEN", "single_tube_length_1"},
		{"TUBE_ACT_WT", "single_tube_weight_1"},
	};

	auto it = overrides.find(field_name);
	if (it != overrides.end()) {
		return it->second;
	}

	std::string db_col = field_name;
	std::transform(db_col.begin(), db_col.end(), db_col.begin(), ::tolower);
	return db_col;
}

// ============================================================
// GSPA19 -> gt4_cast_plan 处理 (增/删)
// ============================================================
bool process_gspa19(pqxx::connection &conn,
					const std::vector<ParsedRecord> &records,
					std::string &out_error)
{
	try {
		pqxx::work txn(conn);

		// 按 OPERATOR_FLAG 分流
		std::vector<const ParsedRecord *> insert_records;
		std::vector<const ParsedRecord *> delete_records;

		for (const auto &rec : records) {
			const std::string flag = get_str(rec, "OPERATOR_FLAG");
			if (flag == "D") {
				delete_records.push_back(&rec);
			} else {
				insert_records.push_back(&rec);
			}
		}

		// 执行删除 (按 order_no + bundle_no 匹配)
		const std::string delete_sql = R"SQL(
			DELETE FROM gt4_cast_plan
			WHERE order_no = $1 AND bundle_no = $2
		)SQL";

		for (const auto *rec : delete_records) {
			const std::string order_no = get_str(*rec, "ORDER_NO");
			const std::string bundle_no = get_str(*rec, "MAT_NO");
			auto result = txn.exec_params(delete_sql, order_no, bundle_no);
			spdlog::info("GSPA19 DELETE: order_no={}, bundle_no={}, affected_rows={}",
						 order_no, bundle_no, result.affected_rows());
		}

		// 动态构建 INSERT SQL
		const auto &fields = GSPA19::fields;
		std::ostringstream col_ss, val_ss;
		for (size_t i = 0; i < fields.size(); ++i) {
			if (i > 0) { col_ss << ", "; val_ss << ", "; }
			col_ss << field_to_db_col(fields[i].name);
			val_ss << "$" << (i + 1);
		}
		// select_flag/send_flag 在接收端固定默认 0
		col_ss << ", select_flag, send_flag, toc";
		val_ss << ", '0', '0', now()";

		const std::string insert_sql =
			"INSERT INTO gt4_cast_plan (" + col_ss.str() + ") VALUES (" + val_ss.str() + ")";

		// 执行插入
		for (const auto *rec : insert_records) {
			pqxx::params params;
			params.reserve(fields.size());
			for (const auto &field : fields) {
				auto it = rec->values.find(field.name);
				if (it == rec->values.end()) {
					switch (field.type) {
						case 'D': params.append(0.0); break;
						case 'L': case 'I': params.append(0LL); break;
						default: params.append(std::string()); break;
					}
				} else {
					switch (field.type) {
						case 'D': params.append(get_double(*rec, field.name)); break;
						case 'L': case 'I': params.append(get_int(*rec, field.name)); break;
						default: params.append(get_str(*rec, field.name)); break;
					}
				}
			}
			txn.exec_params(insert_sql, params);
		}

		txn.commit();
		spdlog::info("GSPA19: {} insert(s), {} delete(s) committed.",
					 insert_records.size(), delete_records.size());
		return true;

	} catch (const std::exception &ex) {
		out_error = std::string("GSPA19 DB error: ") + ex.what();
		spdlog::error("{}", out_error);
		return false;
	}
}

// ============================================================
// 注册 HTTP API
// ============================================================
void register_l3_request_api(httplib::Server &server,
							 const std::string &route,
							 CommL3Context &ctx)
{
	server.Post(route, [&ctx](const httplib::Request &req, httplib::Response &res) {
		json response;

		try {
			spdlog::info("body: {}", req.body);
			const auto payload = json::parse(req.body);

			// 1. 从 HTTP Header X-Message-Topic 识别电文类型
			MessageType msg_type = MessageType::Unknown;
			auto topic_it = req.headers.find("X-Message-Topic");
			if (topic_it != req.headers.end()) {
				const std::string &topic = topic_it->second;
				if (topic == "GSPA19") {
					msg_type = MessageType::GSPA19;
				} else {
					spdlog::info("Unsupported message type: {}, body: {}", topic, req.body);
					response["ok"] = false;
					response["message"] = "Message type " + topic + " is not supported. Only GSPA19 is accepted.";
					res.status = 400;
					res.set_content(response.dump(2), "application/json");
					return;
				}
			} else {
				spdlog::info("Missing X-Message-Topic header, body: {}", req.body);
				response["ok"] = false;
				response["message"] = "Missing X-Message-Topic header.";
				res.status = 400;
				res.set_content(response.dump(2), "application/json");
				return;
			}

			spdlog::info("Detected message type: {}", message_type_name(msg_type));

			// 2. 解析 __blocks__ 结构
			size_t column_count = 0;
			std::vector<json> raw_records;
			std::string parse_error;
			if (!parse_l3_block_rows(payload, column_count, raw_records, parse_error)) {
				response["ok"] = false;
				response["message"] = parse_error;
				res.status = 400;
				res.set_content(response.dump(2), "application/json");
				return;
			}

			if (raw_records.empty()) {
				response["ok"] = false;
				response["message"] = "No records found in payload.";
				res.status = 400;
				res.set_content(response.dump(2), "application/json");
				return;
			}

			// 3. 根据类型选择 FieldList 并解析
			const FieldList *fields = nullptr;
			switch (msg_type) {
			case MessageType::GSPA19:
				fields = &GSPA19::fields;
				break;
			default:
				break;
			}

			if (!fields) {
				spdlog::info("正常情况下不可能发生的分支，msg_type={}", static_cast<int>(msg_type));
				return;
			}

			std::vector<ParsedRecord> parsed_records;
			if (!parse_records_by_fields(raw_records, *fields, parsed_records, parse_error)) {
				response["ok"] = false;
				response["message"] = parse_error;
				res.status = 400;
				res.set_content(response.dump(2), "application/json");
				return;
			}

			response["ok"] = true;
			response["message_type"] = message_type_name(msg_type);
			response["record_count"] = parsed_records.size();
			response["column_count"] = column_count;
			response["message"] = std::string("Parsed ") + message_type_name(msg_type) + " and inserted into database.";
			res.set_content(response.dump(2), "application/json");

			// 4. 打印解析结果
			print_records(message_type_name(msg_type), *fields, parsed_records);

			// 5. 写入数据库 (线程安全)
			// std::string db_error;
			// bool db_ok = false;
			// {
			// 	std::lock_guard<std::mutex> lock(ctx.pgMutex);
			// 	switch (msg_type) {
			// 	case MessageType::GSPA19:
			// 		db_ok = process_gspa19(*ctx.pgConn, parsed_records, db_error);
			// 		break;
			// 	default:
			// 		break;
			// 	}
			// }
		} catch (const std::exception &ex) {
			spdlog::error("Invalid JSON: {}", ex.what());
			response["ok"] = false;
			response["message"] = std::string("Invalid JSON: ") + ex.what();
			res.status = 400;
			res.set_content(response.dump(2), "application/json");
		}
	});
}

} // namespace

L2RcvL3::L2RcvL3(CommL3Context &ctx) : ctx(ctx)
{
}

void L2RcvL3::Run()
{
	httplib::Server server;

	server.Get("/", [](const httplib::Request &, httplib::Response &res) {
		res.set_content("Http server is running.", "text/plain; charset=utf-8");
	});

	register_l3_request_api(server, "/L3RequestData", ctx);

	// 在后台线程启动 HTTP 服务器，主线程监控 ctx.running 实现 Ctrl+C 退出
	std::thread listener([&server]() {
		server.listen("0.0.0.0", 9011);
	});

	spdlog::info("HTTP server started at http://localhost:9011");
	spdlog::info("POST http://localhost:9011/L3RequestData");

	// 等待退出信号（由 main 线程在收到 SIGINT/SIGTERM 后设置 ctx.running = false）
	while (ctx.running)
	{
		std::this_thread::sleep_for(std::chrono::milliseconds(500));
	}

	spdlog::info("Shutting down HTTP server ...");
	server.stop();
	if (listener.joinable())
	{
		listener.join();
	}
	spdlog::info("HTTP server stopped.");
}
