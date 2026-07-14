#include "L2RcvL3.h"

#include "MesStruct.h"
#include "logging.h"
#include <algorithm>
#include <sstream>
#include <stdexcept>
#include <string>
#include <unordered_map>
#include <variant>
#include <vector>

#include <httplib.h>
#include <nlohmann/json.hpp>

namespace
{

	using json = nlohmann::json;

	using GSPA19Value = std::variant<std::string, long long, double>;

	struct GSPA19Data
	{
		std::unordered_map<std::string, GSPA19Value> values;
	};

	// std::string gspa19_value_to_string(const GSPA19Value &value)
	// {
	// 	return std::visit([](const auto &v)
	// 					  {
	// 	std::ostringstream oss;
	// 	oss << v;
	// 	return oss.str(); }, value);
	// }

	// GSPA19Value parse_value_by_type(const json &value, char field_type)
	// {
	// 	if (value.is_null())
	// 	{
	// 		if (field_type == 'D')
	// 		{
	// 			return 0.0;
	// 		}
	// 		if (field_type == 'L')
	// 		{
	// 			return 0LL;
	// 		}
	// 		return std::string();
	// 	}

	// 	if (field_type == 'D')
	// 	{
	// 		if (value.is_number())
	// 		{
	// 			return value.get<double>();
	// 		}
	// 		if (value.is_string())
	// 		{
	// 			return std::stod(value.get<std::string>());
	// 		}
	// 		throw std::runtime_error("Expected numeric/decimal value");
	// 	}

	// 	if (field_type == 'L')
	// 	{
	// 		if (value.is_number_integer())
	// 		{
	// 			return value.get<long long>();
	// 		}
	// 		if (value.is_number_float())
	// 		{
	// 			return static_cast<long long>(value.get<double>());
	// 		}
	// 		if (value.is_string())
	// 		{
	// 			return std::stoll(value.get<std::string>());
	// 		}
	// 		throw std::runtime_error("Expected integer value");
	// 	}

	// 	if (value.is_string())
	// 	{
	// 		return value.get<std::string>();
	// 	}
	// 	return value.dump();
	// }

	// bool parse_l3_block_rows(const json &payload,
	// 						 size_t &out_column_count,
	// 						 std::vector<json> &out_records,
	// 						 std::string &out_error)
	// {
	// 	if (!payload.is_object())
	// 	{
	// 		out_error = "Request body must be a JSON object.";
	// 		return false;
	// 	}

	// 	auto blocks_it = payload.find("__blocks__");
	// 	if (blocks_it == payload.end() || !blocks_it->is_object())
	// 	{
	// 		out_error = "Missing object: __blocks__.";
	// 		return false;
	// 	}

	// 	auto default0_it = blocks_it->find("Default0");
	// 	if (default0_it == blocks_it->end() || !default0_it->is_object())
	// 	{
	// 		out_error = "Missing object: __blocks__.Default0.";
	// 		return false;
	// 	}

	// 	auto meta_it = default0_it->find("meta");
	// 	auto rows_it = default0_it->find("rows");
	// 	if (meta_it == default0_it->end() || !meta_it->is_object())
	// 	{
	// 		out_error = "Missing object: __blocks__.Default0.meta.";
	// 		return false;
	// 	}
	// 	if (rows_it == default0_it->end() || !rows_it->is_array())
	// 	{
	// 		out_error = "Missing array: __blocks__.Default0.rows.";
	// 		return false;
	// 	}

	// 	auto columns_it = meta_it->find("columns");
	// 	if (columns_it == meta_it->end() || !columns_it->is_array())
	// 	{
	// 		out_error = "Missing array: __blocks__.Default0.meta.columns.";
	// 		return false;
	// 	}

	// 	std::vector<std::string> pos_to_name;
	// 	for (size_t idx = 0; idx < columns_it->size(); ++idx)
	// 	{
	// 		const auto &col = (*columns_it)[idx];
	// 		if (!col.is_object())
	// 		{
	// 			continue;
	// 		}

	// 		auto name_it = col.find("name");
	// 		if (name_it == col.end() || !name_it->is_string())
	// 		{
	// 			continue;
	// 		}

	// 		int pos = static_cast<int>(idx);
	// 		auto pos_it = col.find("pos");
	// 		if (pos_it != col.end() && pos_it->is_number_integer())
	// 		{
	// 			pos = pos_it->get<int>();
	// 		}

	// 		if (pos < 0)
	// 		{
	// 			continue;
	// 		}

	// 		if (static_cast<size_t>(pos) >= pos_to_name.size())
	// 		{
	// 			pos_to_name.resize(static_cast<size_t>(pos) + 1);
	// 		}
	// 		pos_to_name[static_cast<size_t>(pos)] = name_it->get<std::string>();
	// 	}

	// 	out_column_count = std::count_if(pos_to_name.begin(), pos_to_name.end(), [](const std::string &name)
	// 									 { return !name.empty(); });

	// 	for (size_t row_idx = 0; row_idx < rows_it->size(); ++row_idx)
	// 	{
	// 		const auto &row = (*rows_it)[row_idx];
	// 		if (!row.is_array())
	// 		{
	// 			out_error = "Each item in __blocks__.Default0.rows must be an array.";
	// 			return false;
	// 		}

	// 		json record = json::object();
	// 		for (size_t col_idx = 0; col_idx < row.size(); ++col_idx)
	// 		{
	// 			std::string key;
	// 			if (col_idx < pos_to_name.size() && !pos_to_name[col_idx].empty())
	// 			{
	// 				key = pos_to_name[col_idx];
	// 			}
	// 			else
	// 			{
	// 				key = "COL_" + std::to_string(col_idx);
	// 			}
	// 			record[key] = row[col_idx];
	// 		}
	// 		out_records.push_back(std::move(record));
	// 	}

	// 	return true;
	// }

	// bool parse_gspa19_records(const std::vector<json> &raw_records,
	// 						  std::vector<GSPA19Data> &out_records,
	// 						  std::string &out_error)
	// {
	// 	for (size_t row_idx = 0; row_idx < raw_records.size(); ++row_idx)
	// 	{
	// 		const auto &raw = raw_records[row_idx];
	// 		GSPA19Data record;

	// 		for (const auto &field : GSPA19::fields)
	// 		{
	// 			auto it = raw.find(field.name);
	// 			if (it == raw.end())
	// 			{
	// 				continue;
	// 			}

	// 			try
	// 			{
	// 				record.values[field.name] = parse_value_by_type(*it, field.type);
	// 			}
	// 			catch (const std::exception &ex)
	// 			{
	// 				out_error = "Row " + std::to_string(row_idx) + ", field " + field.name +
	// 							": " + ex.what();
	// 				return false;
	// 			}
	// 		}

	// 		out_records.push_back(std::move(record));
	// 	}

	// 	return true;
	// }

	// void print_gspa19_records(const std::vector<GSPA19Data> &records)
	// {
	// 	spdlog::info("========== GSPA19 ==========");
	// 	spdlog::info("record_count={}", records.size());

	// 	for (size_t i = 0; i < records.size(); ++i)
	// 	{
	// 		spdlog::info("-- record[{}] --", i);
	// 		for (const auto &field : GSPA19::fields)
	// 		{
	// 			auto it = records[i].values.find(field.name);
	// 			if (it == records[i].values.end())
	// 			{
	// 				spdlog::info("{}=<missing>", field.name);
	// 			}
	// 			else
	// 			{
	// 				spdlog::info("{}={}", field.name, gspa19_value_to_string(it->second));
	// 			}
	// 		}
	// 	}

	// 	spdlog::info("==============================");
	// }

	void register_l3_request_api(httplib::Server &server, const std::string &route)
	{
		server.Post(route, [](const httplib::Request &req, httplib::Response &res) {
			json response;

			// try
			// {
			// 	spdlog::info("body: {}", req.body);
			// 	const auto payload = json::parse(req.body);

			// 	size_t column_count = 0;
			// 	std::vector<json> raw_records;
			// 	std::string parse_error;
			// 	if (!parse_l3_block_rows(payload, column_count, raw_records, parse_error))
			// 	{
			// 		response["ok"] = false;
			// 		response["message"] = parse_error;
			// 		res.status = 400;
			// 		res.set_content(response.dump(2), "application/json");
			// 		return;
			// 	}

			// 	std::vector<GSPA19Data> gspa19_records;
			// 	if (!parse_gspa19_records(raw_records, gspa19_records, parse_error))
			// 	{
			// 		response["ok"] = false;
			// 		response["message"] = parse_error;
			// 		res.status = 400;
			// 		res.set_content(response.dump(2), "application/json");
			// 		return;
			// 	}

			// 	print_gspa19_records(gspa19_records);

			// 	response["ok"] = true;
			// 	response["message_type"] = "GSPA19";
			// 	response["record_count"] = gspa19_records.size();
			// 	response["column_count"] = column_count;
			// 	response["message"] = "Parsed GSPA19 and wrote logs.";
			// 	res.set_content(response.dump(2), "application/json");
			// }
			// catch (const std::exception &ex)
			// {
			// 	spdlog::error("Invalid JSON: {}", ex.what());
			// 	response["ok"] = false;
			// 	response["message"] = std::string("Invalid JSON: ") + ex.what();
			// 	res.status = 400;
			// 	res.set_content(response.dump(2), "application/json");
			// }
		});
	}
} // namespace

L2RcvL3::L2RcvL3(CommL3Context &ctx) : ctx(ctx)
{
}

void L2RcvL3::Run()
{
	httplib::Server server;

	server.Get("/", [](const httplib::Request &, httplib::Response &res)
			   { res.set_content("TestHttp server is running.", "text/plain; charset=utf-8"); });

	register_l3_request_api(server, "/L3RequestData");

	spdlog::info("HTTP server started at http://localhost:9011");
	spdlog::info("POST http://localhost:9011/L3RequestData");

	server.listen("0.0.0.0", 9011);
}