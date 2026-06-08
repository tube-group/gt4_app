#include "L2SndL3.h"

#include "logging.h"

#include <algorithm>
#include <cctype>
#include <functional>
#include <iostream>
#include <utility>
#include <vector>

namespace {

using json = nlohmann::json;

std::string field_type_to_http_type(char field_type)
{
	if (field_type == 'D' || field_type == 'L' || field_type == 'I') {
		return "N";
	}
	return "C";
}

json ensure_rows_array(const json &rows)
{
	if (!rows.is_array()) {
		return json::array();
	}
	return rows;
}

std::string to_upper_copy(std::string value)
{
	std::transform(value.begin(), value.end(), value.begin(), [](unsigned char ch) {
		return static_cast<char>(std::toupper(ch));
	});
	return value;
}

std::string trim_copy(std::string value)
{
	const auto begin = value.find_first_not_of(" \t\r\n");
	if (begin == std::string::npos) {
		return "";
	}

	const auto end = value.find_last_not_of(" \t\r\n");
	return value.substr(begin, end - begin + 1);
}

json build_gsp819_rows()
{
	json rows = json::array();
	rows.push_back(json::array({"I", "GT42600158", "8681600", "641772", "16305426", "80413", 163.00, 27.00, 0.00, "6801", "29CrMo", "1G02", "BG125V", "Q/BQB 267-2023", "4", "NU", "a", "BGT2", 6, 6.033000, 63.600, "5", "水淬&回火", "E059", "B", "接箍料", 0.000, 0.000, "9800100", 13.100, 0.571000}));
	rows.push_back(json::array({"I", "GT42600158", "8681600", "641772", "16305426", "80413", 163.00, 27.00, 0.00, "6801", "29CrMo", "1G02", "BG125V", "Q/BQB 267-2023", "4", "NU", "a", "BGT2", 6, 6.033000, 63.600, "5", "水淬&回火", "E059", "B", "接箍料", 0.000, 0.000, "9800110", 13.100, 0.571000}));
	rows.push_back(json::array({"I", "GT42600158", "8681600", "641772", "16305426", "80413", 163.00, 27.00, 0.00, "6801", "29CrMo", "1G02", "BG125V", "Q/BQB 267-2023", "4", "NU", "a", "BGT2", 6, 6.033000, 63.600, "5", "水淬&回火", "E059", "B", "接箍料", 0.000, 0.000, "9800120", 13.100, 0.571000}));
	rows.push_back(json::array({"I", "GT42600158", "8681600", "641772", "16305426", "80413", 163.00, 27.00, 0.00, "6801", "29CrMo", "1G02", "BG125V", "Q/BQB 267-2023", "4", "NU", "a", "BGT2", 6, 6.033000, 63.600, "5", "水淬&回火", "E059", "B", "接箍料", 0.000, 0.000, "9800130", 13.100, 0.571000}));
	rows.push_back(json::array({"I", "GT42600158", "8681600", "641772", "16305426", "80413", 163.00, 27.00, 0.00, "6801", "29CrMo", "1G02", "BG125V", "Q/BQB 267-2023", "4", "NU", "a", "BGT2", 6, 6.033000, 63.600, "5", "水淬&回火", "E059", "B", "接箍料", 0.000, 0.000, "9800140", 13.100, 0.571000}));
	return rows;
}

json build_gspa19_rows()
{
	json rows = json::array();
	rows.push_back(json::array({"I", "ORDER20260528011", "MAT-X", "RL-011", "PO-011", "LOT-011", 5.50, 0.35, "SG-X", "STD-X", "API", "BTC", 5, 980.200000, 5200.000, "TX01", 5.200, 990.000000}));
	rows.push_back(json::array({"I", "ORDER20260528012", "MAT-Y", "RL-012", "PO-012", "LOT-012", 6.00, 0.38, "SG-Y", "STD-Y", "PREM", "LTC", 6, 1055.400000, 5400.000, "TY02", 5.400, 1066.800000}));
	rows.push_back(json::array({"I", "ORDER20260528013", "MAT-Z", "RL-013", "PO-013", "LOT-013", 6.50, 0.42, "SG-Z", "STD-Z", "STD", "STC", 7, 1128.900000, 5600.000, "TZ03", 5.600, 1140.300000}));
	return rows;
}

std::unordered_map<std::string, json> build_gsp813_values()
{
	return {
		{"OPERATOR_FLAG", "I"},
		{"PROD_DATE", "20260528"},
		{"PROD_TM", "090738"},
		{"PROD_SHIFT_NO", "2"},
		{"PROD_SHIFT_GROUP", "2"},
		{"PROD_JOB_POINT", "E101"},
		{"DEST_CODE", "E059"},
		{"ORDER_NO", "GT42600158"},
		{"ORDER_OUTER_DIA", 163.00},
		{"ORDER_WAL_THICK", 27.00},
		{"ORDER_HEIGHT", 3.00},
		{"MTRL_NO", "6801"},
		{"MTRL_TEXT", "29CrMo"},
		{"STD_SG_CODE", "1G02"},
		{"SG_STD", "Q/BQB 267-2023"},
		{"SG_SIGN", "BG125V"},
		{"PIPEEND_TYPE_CODE", "4"},
		{"PIPEEND_TYPE_SIGN", "NU"},
		{"THREAD_TYPE_CODE", "a"},
		{"THREAD_TYPE_SIGN", "BGT2"},
		{"HEAT_TREAT_STATUS_CODE", "5"},
		{"OLD_ORDER_NO", "GT42600157"},
		{"RL_NO", "641772"},
		{"PONO", "16305426"},
		{"SAMPLE_LOT_NO", "80413"},
		{"MAT_NO", "8681600"},
		{"BUNDLE_TYPE", "001"},
		{"MAT_TUBE", 6},
		{"MAT_ACT_WT", 6.033000},
		{"LEN_FROM", 12.000},
		{"LEN_TO", 12.000},
		{"TOTAL_LEN", 63.600},
		{"STOCK_PLACE_NO", "test"},
		{"MAT_NO_OLD", "8681500"},
		{"TUBE_NO", "9800140"},
		{"TUBE_ACT_WT", 12.571000},
		{"TUBE_LEN", 13}
	};
}

std::unordered_map<std::string, json> build_gsp831_values()
{
	return {
		{"BREAKDN_DATE", "20260528"},
		{"DEV_CODE", "E601"},
		{"DEV_NAME", "test"},
		{"WORK_LINE_CODE", "L3"},
		{"WORK_LINE_NAME", "test"},
		{"ACC_START_TIME", "20260528101530"},
		{"ACC_END_TIME", "20260528104530"},
		{"TOTAL_BREAKDN_TIME", 30.00},
		{"BREAKDN_TYPE_CODE", "01"},
		{"BREAKDN_TYPE_TEXT", "test"},
		{"BREAKDN_START_GRP", "A"},
		{"REMARK", "test"}
	};
}

std::unordered_map<std::string, json> build_gspa13_values()
{
	return {
		{"OPERATOR_FLAG", "I"}, {"PROD_DATE", "20260501"}, {"PROD_TM", "090738"},
		{"PROD_SHIFT_NO", "2"}, {"PROD_SHIFT_GROUP", "2"}, {"PROD_JOB_POINT", "E101"},
		{"DEST_CODE", "T310"}, {"ORDER_NO", "GT42600159"}, {"ORDER_ITEM_NO", "000"},
		{"PROD_CODE", "A"}, {"PROD_CNAME", "品名细分类中文"}, {"ORDER_OUTER_DIA", 111.11},
		{"ORDER_WAL_THICK", 10.36}, {"ORDER_HEIGHT", 123.12}, {"STD_SG_CODE", "4L06"},
		{"SG_SIGN", "L80-1"}, {"SG_STD", "API SPEC 5CT-2023 11th Edition,E1 May 2024,E2 Aug. 2024"},
		{"MTRL_NO", "3856"}, {"MTRL_TEXT", "29Mn2"}, {"PIPEEND_TYPE_CODE", "4"},
		{"PIPEEND_TYPE_SIGN", "NU"}, {"PIPEEND_TYPE", "test"}, {"THREAD_TYPE_CODE", "a"},
		{"THREAD_TYPE_SIGN", "BGT2"}, {"THREAD_TYPE", "BGT2"}, {"OLD_ORDER_NO", "GT42600159"},
		{"RL_NO", "642118"}, {"PONO", "16305411"}, {"SAMPLE_LOT_NO", "81285"},
		{"MAT_NO", "2020453"}, {"BUNDLE_TYPE", "000"}, {"MAT_TUBE", 3},
		{"MAT_ACT_WT", 1234.123456}, {"MAT_ENG_WT", 1230.654321}, {"TOTAL_LEN", 23.111},
		{"ENG_TOTAL_LEN", 34.222}, {"LEN_FROM", 56.333}, {"LEN_TO", 78.444},
		{"MAT_THEORY_WT", 7896.654321}, {"THY_TOTAL_LEN", 394400}, {"ORDER_END_FLAG", "0"},
		{"CTL_TUBE_DIA_FROM", 123.456}, {"CTL_TUBE_DIA_TO", 179.310}, {"CTL_WAL_THICK_FROM", 9.12},
		{"CTL_WAL_THICK_TO", 11.18}, {"CTL_HEIGHT_FROM", 0.000}, {"CTL_HEIGHT_TO", 0.456},
		{"COUP_PONO", "16304288"}, {"COUP_SAMPLE_LOT_NO", "80404"}, {"PROTECT_SIZE", "test"},
		{"PROTECT_LOT_NO", "test"}, {"PROTECT_FACTORY", "test"}, {"COMPOUND_SIZE", "test"},
		{"COMPOUND_LOT_NO", "test"}, {"COMPOUND_FACTORY", "test"}, {"COATING_SIZE", "test"},
		{"COATING_LOT_NO", "test"}, {"COATING_FACTORY", "test"}, {"STOCK_PLACE_NO", "test"},
		{"TUBE_NO", "test"}, {"TUBE_ACT_WT", 11.000001}, {"TUBE_ENG_WT", 22.000002},
		{"TUBE_THEORY_WT", 33.000003}, {"TUBE_LEN", 400}, {"TUBE_ENG_LEN", 400.004},
		{"TUBE_THY_LEN", 400}, {"SEQ_ID", 17311}
	};
}

std::unordered_map<std::string, json> build_gspa31_values()
{
	return {
		{"BREAKDN_DATE", "20260528"},
		{"DEV_CODE", "A601"},
		{"DEV_NAME", "test"},
		{"WORK_LINE_CODE", "A3"},
		{"WORK_LINE_NAME", "test"},
		{"ACC_START_TIME", "20260528111530"},
		{"ACC_END_TIME", "20260528113030"},
		{"TOTAL_BREAKDN_TIME", 15.00},
		{"BREAKDN_TYPE_CODE", "02"},
		{"BREAKDN_TYPE_TEXT", "test"},
		{"BREAKDN_START_GRP", "B"},
		{"REMARK", "test"}
	};
}

struct TestCaseEntry
{
	std::string code;
	std::string description;
	std::function<void()> handler;
};

std::vector<TestCaseEntry> build_test_cases(L2SndL3 &sender)
{
	return {
		{"GSP819", "POST multi-row to /L3RequestData", [&sender]() {
			sender.SendRowsToL3RequestData("GSP819", GSP819::fields, build_gsp819_rows(), true, "multi-row GSP819 test");
		}},
		{"GSPA19", "POST multi-row to /L3RequestData", [&sender]() {
			sender.SendRowsToL3RequestData("GSPA19", GSPA19::fields, build_gspa19_rows(), true, "multi-row GSPA19 test");
		}},
		{"GSP813", "POST single-row to /api/v1/restin", [&sender]() {
			sender.SendSingleRowToRestIn("GSP813", GSP813::fields, build_gsp813_values(), true, "single-row GSP813 test");
		}},
		{"GSP831", "POST single-row to /api/v1/restin", [&sender]() {
			sender.SendSingleRowToRestIn("GSP831", GSP831::fields, build_gsp831_values(), true, "single-row GSP831 test");
		}},
		{"GSPA13", "POST single-row to /api/v1/restin", [&sender]() {
			sender.SendSingleRowToRestIn("GSPA13", GSPA13::fields, build_gspa13_values(), true, "single-row GSPA13 test");
		}},
		{"GSPA31", "POST single-row to /api/v1/restin", [&sender]() {
			sender.SendSingleRowToRestIn("GSPA31", GSPA31::fields, build_gspa31_values(), true, "single-row GSPA31 test");
		}}
	};
}

void print_test_menu(const std::vector<TestCaseEntry> &cases)
{
	std::cout << "\n===== L2SndL3 Test Menu =====" << std::endl;
	std::cout << "Input telegram no (or EXIT):" << std::endl;
	for (const auto &entry : cases) {
		std::cout << entry.code << "  -> " << entry.description << std::endl;
	}
	std::cout << "Please input: ";
}

std::string read_test_command()
{
	std::string input;
	if (!std::getline(std::cin, input)) {
		return "";
	}

	return to_upper_copy(trim_copy(input));
}

} // namespace

L2SndL3::L2SndL3(CommL3Context &ctx)
	: ctx(ctx)
	, l3_request_data_url_("http://140.32.1.189:9011/L3RequestData")
	, rest_in_url_("http://140.32.1.189:10007/api/v1/restin")
{
}

void L2SndL3::Run()
{
	spdlog::info("L2SndL3 ready: L3RequestData={}, RestIn={}", l3_request_data_url_, rest_in_url_);
}

void L2SndL3::SetL3RequestDataUrl(const std::string &url)
{
	l3_request_data_url_ = url;
}

void L2SndL3::SetRestInUrl(const std::string &url)
{
	rest_in_url_ = url;
}

json L2SndL3::BuildRow(const FieldList &fields,
					   const std::unordered_map<std::string, json> &values) const
{
	json row = json::array();

	for (const auto &field : fields) {
		auto it = values.find(field.name);
		if (it == values.end()) {
			row.push_back("");
		} else {
			row.push_back(it->second);
		}
	}

	return row;
}

json L2SndL3::BuildPayload(const std::string &service_id,
						   const FieldList &fields,
						   const json &rows,
						   bool include_types) const
{
	json column_defs = json::array();

	for (size_t i = 0; i < fields.size(); ++i) {
		json column = {
			{"name", fields[i].name},
			{"pos", static_cast<int>(i)}
		};

		if (include_types) {
			column["type"] = field_type_to_http_type(fields[i].type);
		}

		column_defs.push_back(std::move(column));
	}

	return {
		{"ICMS_MESSAGE_NAME", service_id},
		{"__sys__", {{"serviceId", service_id}}},
		{"__blocks__", {
			{"Default0", {
				{"meta", {{"columns", column_defs}}},
				{"rows", ensure_rows_array(rows)}
			}}
		}}
	};
}

cpr::Response L2SndL3::PostMessage(const std::string &url,
								   const std::string &topic,
								   const json &payload,
								   const std::string &case_name) const
{
	if (!case_name.empty()) {
		spdlog::info("[SEND] {}", case_name);
	}
	spdlog::info("[SEND] POST {}", url);
	spdlog::info("[SEND] topic={}", topic);
	spdlog::info("[SEND] body={}", payload.dump());

	const auto response = cpr::Post(
		cpr::Url{url},
		cpr::Header{{"Content-Type", "application/json"},
					{"X-Message-Topic", topic},
					{"X-Meaaage-Topic", topic}},
		cpr::Body{payload.dump()});

	spdlog::info("[SEND] status_code={}", response.status_code);
	if (response.error.code != cpr::ErrorCode::OK) {
		spdlog::error("[SEND] error={}", response.error.message);
	}
	spdlog::info("[SEND] response={}", response.text);

	return response;
}

cpr::Response L2SndL3::PostToL3RequestData(const std::string &topic,
										   const json &payload,
										   const std::string &case_name) const
{
	return PostMessage(l3_request_data_url_, topic, payload, case_name);
}

cpr::Response L2SndL3::PostToRestIn(const std::string &topic,
									const json &payload,
									const std::string &case_name) const
{
	return PostMessage(rest_in_url_, topic, payload, case_name);
}

cpr::Response L2SndL3::SendRowsToL3RequestData(const std::string &service_id,
											   const FieldList &fields,
											   const json &rows,
											   bool include_types,
											   const std::string &case_name) const
{
	const auto payload = BuildPayload(service_id, fields, rows, include_types);
	return PostToL3RequestData(service_id, payload, case_name);
}

cpr::Response L2SndL3::SendRowsToRestIn(const std::string &service_id,
										const FieldList &fields,
										const json &rows,
										bool include_types,
										const std::string &case_name) const
{
	const auto payload = BuildPayload(service_id, fields, rows, include_types);
	return PostToRestIn(service_id, payload, case_name);
}

cpr::Response L2SndL3::SendSingleRowToL3RequestData(
	const std::string &service_id,
	const FieldList &fields,
	const std::unordered_map<std::string, json> &values,
	bool include_types,
	const std::string &case_name) const
{
	json rows = json::array();
	rows.push_back(BuildRow(fields, values));
	return SendRowsToL3RequestData(service_id, fields, rows, include_types, case_name);
}

cpr::Response L2SndL3::SendSingleRowToRestIn(
	const std::string &service_id,
	const FieldList &fields,
	const std::unordered_map<std::string, json> &values,
	bool include_types,
	const std::string &case_name) const
{
	json rows = json::array();
	rows.push_back(BuildRow(fields, values));
	return SendRowsToRestIn(service_id, fields, rows, include_types, case_name);
}

void L2SndL3::Test()
{
	const auto cases = build_test_cases(*this);

	while (ctx.running.load()) {
		print_test_menu(cases);
		const auto code = read_test_command();

		if (code.empty()) {
			if (!std::cin.good()) {
				std::cout << "Input closed, exit test mode." << std::endl;
				break;
			}
			std::cout << "Invalid input, please retry." << std::endl;
			continue;
		}

		if (code == "0" || code == "EXIT" || code == "QUIT") {
			std::cout << "Bye." << std::endl;
			break;
		}

		auto it = std::find_if(cases.begin(), cases.end(), [&code](const TestCaseEntry &entry) {
			return entry.code == code;
		});

		if (it == cases.end()) {
			std::cout << "Unknown telegram: " << code << std::endl;
			continue;
		}

		try {
			it->handler();
		} catch (const std::exception &ex) {
			spdlog::error("Test send failed: {}", ex.what());
			std::cout << "Send failed: " << ex.what() << std::endl;
		}
	}
}
