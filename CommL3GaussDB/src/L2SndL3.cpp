#include "L2SndL3.h"

#include "logging.h"

#include <algorithm>
#include <cctype>
#include <functional>
#include <iostream>
#include <utility>
#include <vector>

namespace
{
	using json = nlohmann::json;

	std::string field_type_to_http_type(char field_type)
	{
		if (field_type == 'D' || field_type == 'L' || field_type == 'I')
		{
			return "N";
		}
		return "C";
	}

	json ensure_rows_array(const json &rows)
	{
		if (!rows.is_array())
		{
			return json::array();
		}
		return rows;
	}
} // namespace

L2SndL3::L2SndL3()
	: rest_in_url_("http://10.81.57.82:10007/api/v1/restin")
{
}

void L2SndL3::SetRestInUrl(const std::string &url)
{
	rest_in_url_ = url;
}

json L2SndL3::BuildRow(const FieldList &fields,
					   const std::unordered_map<std::string, json> &values) const
{
	json row = json::array();

	for (const auto &field : fields)
	{
		auto it = values.find(field.name);
		if (it == values.end())
		{
			row.push_back("");
		}
		else
		{
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

	for (size_t i = 0; i < fields.size(); ++i)
	{
		json column = {
			{"name", fields[i].name},
			{"pos", static_cast<int>(i)}};

		if (include_types)
		{
			column["type"] = field_type_to_http_type(fields[i].type);
		}

		column_defs.push_back(std::move(column));
	}

	return {
		{"ICMS_MESSAGE_NAME", service_id},
		{"__sys__", {{"serviceId", service_id}}},
		{"__blocks__", {{"Default0", {{"meta", {{"columns", column_defs}}}, {"rows", ensure_rows_array(rows)}}}}}};
}

cpr::Response L2SndL3::PostMessage(const std::string &url,
								   const std::string &topic,
								   const json &payload,
								   const std::string &case_name) const
{
	if (!case_name.empty())
	{
		spdlog::info("[SEND] {}", case_name);
	}
	spdlog::info("[SEND] POST {}", url);
	spdlog::info("[SEND] topic={}", topic);
	spdlog::info("[SEND] body={}", payload.dump());

	const auto response = cpr::Post(
		cpr::Url{url},
		cpr::Header{{"Content-Type", "application/json"},
					{"X-Message-Topic", topic}},
		cpr::Body{payload.dump()});

	spdlog::info("[SEND] status_code={}", response.status_code);
	if (response.error.code != cpr::ErrorCode::OK)
	{
		spdlog::error("[SEND] error={}", response.error.message);
	}
	spdlog::info("[SEND] response={}", response.text);

	return response;
}

cpr::Response L2SndL3::PostToRestIn(const std::string &topic,
									const json &payload,
									const std::string &case_name) const
{
	return PostMessage(rest_in_url_, topic, payload, case_name);
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