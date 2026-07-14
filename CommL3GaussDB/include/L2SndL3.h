#pragma once

#include "CommL3Context.h"
#include "MesStruct.h"

#include <cpr/cpr.h>
#include <nlohmann/json.hpp>

#include <string>
#include <unordered_map>

class L2SndL3
{
public:
    L2SndL3();
    void Run();

    void SetL3RequestDataUrl(const std::string &url);
    void SetRestInUrl(const std::string &url);

    nlohmann::json BuildRow(const FieldList &fields,
                            const std::unordered_map<std::string, nlohmann::json> &values) const;

    nlohmann::json BuildPayload(const std::string &service_id,
                                const FieldList &fields,
                                const nlohmann::json &rows,
                                bool include_types = true) const;

    cpr::Response PostMessage(const std::string &url,
                              const std::string &topic,
                              const nlohmann::json &payload,
                              const std::string &case_name = "") const;

    cpr::Response PostToRestIn(const std::string &topic,
                               const nlohmann::json &payload,
                               const std::string &case_name = "") const;

    cpr::Response SendRowsToRestIn(const std::string &service_id,
                                   const FieldList &fields,
                                   const nlohmann::json &rows,
                                   bool include_types = true,
                                   const std::string &case_name = "") const;

    cpr::Response SendSingleRowToRestIn(
        const std::string &service_id,
        const FieldList &fields,
        const std::unordered_map<std::string, nlohmann::json> &values,
        bool include_types = true,
        const std::string &case_name = "") const;

private:
    std::string rest_in_url_;
};
