#include "logging.h"
#include "higplat.h"
#include "CommL3Context.h"
#include "user_types.h"
#include <ctime>
#include <string>
#include "L2SndL3.h"

using json = nlohmann::json;

std::unordered_map<std::string, json> build_gspa13_values(const pqxx::result::reference &row);

void handleApiBundleDataEvent(CommL3Context &ctx, const char *value)
{
    ApiBundleDataEvent bundleEvent = read_value<ApiBundleDataEvent>(value);

    spdlog::info("Processing API_BUNDLE_DATA_EVENT: order_no={}, item_no={}, bundle_no={}, flag={}",
                 bundleEvent.order_no.to_string(), bundleEvent.item_no.to_string(), bundleEvent.bundle_no.to_string(), bundleEvent.flag.to_string());

    std::unordered_map<std::string, json> gspa13_values; // 用于存储gspa13的字段值
    
    double weight_per_meter = 1.0; // 默认值，实际应从数据库或配置中获取

    // 查询数据库表api_bundle_data_t
    try
    {
        pqxx::nontransaction ntx(*ctx.pgConn);
        const pqxx::result result = ntx.exec(
            "SELECT * FROM api_bundle_data_t WHERE order_no = $1 AND item_no = $2 AND bundle_no = $3",
            pqxx::params{bundleEvent.order_no.to_string(), bundleEvent.item_no.to_string(), bundleEvent.bundle_no.to_string()});

        if (result.empty())
        {
            spdlog::warn("No record found in api_bundle_data_t for order_no={}, item_no={}, bundle_no={}",
                         bundleEvent.order_no.to_string(), bundleEvent.item_no.to_string(), bundleEvent.bundle_no.to_string());
            return;
        }

        const auto &row = result[0];

        gspa13_values = build_gspa13_values(row); // 构建 gspa13_values

        weight_per_meter = row["weight_per_meter"].as<double>(); // 米重
    }
    catch (const std::exception &e)
    {
        spdlog::error("操作api_bundle_data_t失败， {}", e.what());
        return;
    }

    // 查询数据库表api_tube_data_t
    try
    {
        pqxx::nontransaction ntx(*ctx.pgConn);
        const pqxx::result result = ntx.exec(
            "SELECT * FROM api_tube_data_t WHERE order_no = $1 AND item_no = $2 AND bundle_no = $3",
            pqxx::params{bundleEvent.order_no.to_string(), bundleEvent.item_no.to_string(), bundleEvent.bundle_no.to_string()});

        if (result.empty())
        {
            spdlog::warn("No record found in api_tube_data_t for order_no={}, item_no={}, bundle_no={}",
                         bundleEvent.order_no.to_string(), bundleEvent.item_no.to_string(), bundleEvent.bundle_no.to_string());
            return;
        }

        L2SndL3 l2sndl3; // 创建 L2SndL3 实例
        json rows = json::array();
        for (int i=0; i<result.size(); ++i)
        {
            const auto &row = result[i];

            gspa13_values["TUBE_NO"] = row["tube_no"].as<std::string>(); // 管号
            gspa13_values["TUBE_ACT_WT"] = row["weight"].as<double>()/1000.0; // 单根管子重量(吨)
            gspa13_values["TUBE_ENG_WT"] = row["weight"].as<double>() * 2.204622; // 单根管子英制重量
            gspa13_values["TUBE_THEORY_WT"] = row["length"].as<double>() * weight_per_meter; // 单根管子理论重量
            gspa13_values["TUBE_LEN"] = row["length"].as<double>(); // 单根管子长度
            gspa13_values["TUBE_ENG_LEN"] = row["length"].as<double>() * 3.280839; // 单根管子英制长度
            gspa13_values["TUBE_THY_LEN"] = row["weight"].as<double>() / weight_per_meter; // 单根管子理论长度
            gspa13_values["SEQ_ID"] = row["flow_no"].as<int>(0); // 流水号

            // rows.push_back(gspa13_values); // 将当前管子数据添加到 rows 数组中
            rows.push_back(l2sndl3.BuildRow(GSPA13::fields, gspa13_values));
        }
        l2sndl3.SendRowsToRestIn("GSPA13", GSPA13::fields, rows, true, "发送HFW套管实绩到L3");
    }
    catch (const std::exception &e)
    {
        spdlog::error("api_tube_data_t {}", e.what());
        return;
    }
}

std::unordered_map<std::string, json> build_gspa13_values(const pqxx::result::reference &row)
{
	return {
		{"OPERATOR_FLAG", "I"}, 
        {"PROD_DATE", row["produce_time"].as<std::string>().substr(0, 8)}, 
        {"PROD_TM", row["produce_time"].as<std::string>().substr(8, 6)},
		{"PROD_SHIFT_NO", row["ban_ci"].as<std::string>().substr(0, 1)}, 
        {"PROD_SHIFT_GROUP", row["ban_ci"].as<std::string>().substr(1)}, 
        {"PROD_JOB_POINT", row["product_job_point"].as<std::string>()},
		{"DEST_CODE", row["direction_code"].as<std::string>()}, 
        {"ORDER_NO", row["order_no"].as<std::string>()}, 
        {"ORDER_ITEM_NO", row["item_no"].as<std::string>()},
		{"PROD_CODE", row["prod_code"].as<std::string>()}, 
        {"PROD_CNAME", row["prod_cname"].as<std::string>()}, 
        {"ORDER_OUTER_DIA", row["diameter"].as<double>()},
		{"ORDER_WAL_THICK", row["wall_thickness"].as<double>()}, 
        {"ORDER_HEIGHT", 0},
        {"STD_SG_CODE", row["std_sg_code"].as<std::string>()},
		{"SG_SIGN", row["sg_text"].as<std::string>()}, 
        {"SG_STD", row["std_text"].as<std::string>()},
		{"MTRL_NO", row["mat_no"].as<std::string>()}, 
        {"MTRL_TEXT", row["mat_text"].as<std::string>()}, 
        {"PIPEEND_TYPE_CODE", row["end_type_code"].as<std::string>()},
		{"PIPEEND_TYPE_SIGN", row["end_type_sign"].as<std::string>()}, 
        {"PIPEEND_TYPE", row["end_type"].as<std::string>()}, 
        {"THREAD_TYPE_CODE", row["thread_type_code"].as<std::string>()},
		{"THREAD_TYPE_SIGN", row["thread_type_sign"].as<std::string>()}, 
        {"THREAD_TYPE", row["thread_type"].as<std::string>()}, 
        {"OLD_ORDER_NO", row["order_no_old"].as<std::string>()},
		{"RL_NO", row["roll_no"].as<std::string>()}, 
        {"PONO", row["melt_no"].as<std::string>()}, 
        {"SAMPLE_LOT_NO", row["lot_no"].as<std::string>()},
		{"MAT_NO", row["bundle_no"].as<std::string>()}, 
        {"BUNDLE_TYPE", row["bundle_type"].as<std::string>()}, 
        {"MAT_TUBE", row["tube"].as<int>()},
		{"MAT_ACT_WT", row["weight"].as<double>()}, 
        {"MAT_ENG_WT", row["weight_eng"].as<double>()}, 
        {"TOTAL_LEN", row["total_length"].as<double>()},
		{"ENG_TOTAL_LEN", row["length_eng"].as<double>()}, 
        {"LEN_FROM", row["length_from"].as<double>()}, 
        {"LEN_TO", row["length_to"].as<double>()},
		{"MAT_THEORY_WT", row["theory_weight"].as<double>()}, 
        {"THY_TOTAL_LEN", row["theory_total_length"].as<double>()}, 
        {"ORDER_END_FLAG", "0"},
		{"CTL_TUBE_DIA_FROM", row["diameter_down_ctrl"].as<double>()}, 
        {"CTL_TUBE_DIA_TO", row["diameter_up_ctrl"].as<double>()}, 
        {"CTL_WAL_THICK_FROM", row["wal_thick_down_ctrl"].as<double>()},
		{"CTL_WAL_THICK_TO", row["wal_thick_up_ctrl"].as<double>()}, 
        {"CTL_HEIGHT_FROM", 0}, 
        {"CTL_HEIGHT_TO", 0},
		{"COUP_PONO", row["pono_id_coupling"].as<std::string>()}, 
        {"COUP_SAMPLE_LOT_NO", row["lot_no_thread"].as<std::string>()}, 
        {"PROTECT_SIZE", " "},
		{"PROTECT_LOT_NO", " "}, 
        {"PROTECT_FACTORY", " "}, 
        {"COMPOUND_SIZE", " "},
		{"COMPOUND_LOT_NO", " "}, 
        {"COMPOUND_FACTORY", " "}, 
        {"COATING_SIZE", " "},
		{"COATING_LOT_NO", " "}, 
        {"COATING_FACTORY", " "}, 
        {"STOCK_PLACE_NO", " "},
		{"TUBE_NO", ""}, 
        {"TUBE_ACT_WT", 0}, 
        {"TUBE_ENG_WT", 0},
		{"TUBE_THEORY_WT", 0}, 
        {"TUBE_LEN", 0}, 
        {"TUBE_ENG_LEN", 0},
		{"TUBE_THY_LEN", 0}, 
        {"SEQ_ID", 0}
	};
}