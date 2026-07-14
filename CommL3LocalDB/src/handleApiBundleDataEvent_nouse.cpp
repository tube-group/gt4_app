#include "logging.h"
#include "higplat.h"
#include "CommL3Context.h"
#include "user_types.h"
#include <ctime>
#include <string>

void handleApiBundleDataEvent(CommL3Context &ctx, const char *value)
{
    ApiBundleDataEvent bundleEvent = read_value<ApiBundleDataEvent>(value);

    spdlog::info("Processing API_BUNDLE_DATA_EVENT: order_no={}, item_no={}, bundle_no={}, flag={}",
                 bundleEvent.order_no.to_string(), bundleEvent.item_no.to_string(), bundleEvent.bundle_no.to_string(), bundleEvent.flag.to_string());

    mmwe_tqmtiwea13_t bundledata{}; // 值初始化，字符串为空，数值字段为零
    
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

        bundledata.operator_flag = bundleEvent.flag.to_string(); // 增删标记
        bundledata.prod_date = row["produce_time"].as<std::string>().substr(0, 8); // 生产日期
        bundledata.prod_tm = row["produce_time"].as<std::string>().substr(8, 6); // 生产时间(6位)
        bundledata.prod_shift_no = row["ban_ci"].as<std::string>().substr(0, 1); // 生产班次
        bundledata.prod_shift_group = row["ban_ci"].as<std::string>().substr(1); // 生产班组
        bundledata.prod_job_point = row["product_job_point"].as<std::string>(); // 生产作业点
        bundledata.dest_code = row["direction_code"].as<std::string>(); // 去向代码
        bundledata.order_no = row["order_no"].as<std::string>(); // 合同号
        bundledata.order_item_no = row["item_no"].as<std::string>(); // 合同项目号
        bundledata.prod_code = row["prod_code"].as<std::string>(); // 品名细分类代码
        bundledata.prod_cname = row["prod_cname"].as<std::string>(); // 品名细分类中文
        bundledata.order_outer_dia = row["diameter"].as<double>(0.0); // 外径
        bundledata.order_wal_thick = row["wall_thickness"].as<double>(0.0); // 壁厚
        bundledata.order_height = 0.0; // 高度
        bundledata.std_sg_code = row["std_sg_code"].as<std::string>(); // 标准牌号(钢级)代码
        bundledata.sg_sign = row["sg_text"].as<std::string>(); // 牌号（钢级）
        bundledata.sg_std = row["std_text"].as<std::string>(); // 牌号标准(标准正文)
        bundledata.mtrl_no = row["mat_no"].as<std::string>(); // 材质号
        bundledata.mtrl_text = row["mat_text"].as<std::string>(); // 材质正文
        bundledata.pipeend_type_code = row["end_type_code"].as<std::string>(); // 管端型式代码
        bundledata.pipeend_type_sign = row["end_type_sign"].as<std::string>(); // 管端型式符号
        bundledata.pipeend_type = row["end_type"].as<std::string>(); // 管端型式
        bundledata.thread_type_code = row["thread_type_code"].as<std::string>(); // 螺纹类型代码
        bundledata.thread_type_sign = row["thread_type_sign"].as<std::string>(); // 螺纹类型符号
        bundledata.thread_type = row["thread_type"].as<std::string>(); // 螺纹类型
        bundledata.old_order_no = row["order_no_old"].as<std::string>(); // 原合同号
        bundledata.rl_no = row["roll_no"].as<std::string>(); // 轧批号
        bundledata.pono = row["melt_no"].as<std::string>(); // 制造命令号(炉号)
        bundledata.sample_lot_no = row["lot_no"].as<std::string>(); // 试批号
        bundledata.mat_no = row["bundle_no"].as<std::string>(); // 材料号(管捆号)
        bundledata.bundle_type = row["bundle_type"].as<std::string>(); // 管捆类型
        bundledata.mat_tube = row["tube"].as<int>(0); // 材料根数
        bundledata.mat_act_wt = row["weight"].as<double>(0.0); // 材料重量(吨)
        bundledata.mat_eng_wt = row["weight_eng"].as<double>(0.0); // 材料英制重量
        bundledata.total_len = row["total_length"].as<double>(0.0); // 总长度
        bundledata.eng_total_len = row["length_eng"].as<double>(0.0); // 英制总长度
        bundledata.len_from = row["length_from"].as<double>(0.0); // 长度起
        bundledata.len_to = row["length_to"].as<double>(0.0); // 长度止
        bundledata.mat_theory_wt = row["theory_weight"].as<int>(0); // 材料理论重量
        bundledata.thy_total_len = row["theory_total_length"].as<double>(0.0); // 理论总长度
        bundledata.order_end_flag = "0"; // 合同结束标记
        bundledata.ctl_tube_dia_from = row["diameter_down_ctrl"].as<double>(0.0); // 内控管体外径起
        bundledata.ctl_tube_dia_to = row["diameter_up_ctrl"].as<double>(0.0); // 内控管体外径止
        bundledata.ctl_wal_thick_from = row["wal_thick_down_ctrl"].as<double>(0.0); // 内控壁厚起
        bundledata.ctl_wal_thick_to = row["wal_thick_up_ctrl"].as<double>(0.0); // 内控壁厚止
        bundledata.ctl_height_from =0.0; // 内控高度起
        bundledata.ctl_height_to = 0.0; // 内控高度止
        bundledata.coup_pono = row["pono_id_coupling"].as<std::string>(); // 接箍炉号
        bundledata.coup_sample_lot_no = row["lot_no_thread"].as<std::string>(); // 接箍试批号
        bundledata.protect_size = " "; // 保护环规格
        bundledata.protect_lot_no = " "; // 保护环批号
        bundledata.protect_factory = " "; // 保护环生产厂家
        bundledata.compound_size = " "; // 螺纹脂规格
        bundledata.compound_lot_no = " "; // 螺纹脂批号
        bundledata.compound_factory = " "; // 螺纹脂生产厂家
        bundledata.coating_size = " "; // 防腐涂层规格
        bundledata.coating_lot_no = " "; // 防腐涂层批号
        bundledata.coating_factory = " "; // 防腐涂层生产厂家
        bundledata.stock_place_no = " "; // 材料库位号

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

        for (int i=0; i<result.size(); ++i)
        {
            const auto &row = result[i];
            // 处理每一条管子数据
            if (i==0)
            {
                bundledata.tube_no_1 = row["tube_no"].as<std::string>(); // 管号1
                bundledata.tube_act_wt_1 = row["weight"].as<double>()/1000.0; // 单根管子重量1(吨)
                bundledata.tube_eng_wt_1 = row["weight"].as<double>() * 2.204622; // 单根管子英制重量1
                bundledata.tube_theory_wt_1 = row["length"].as<double>() * weight_per_meter; // 单根管子理论重量1
                bundledata.tube_len_1 = row["length"].as<double>(); // 单根管子长度1
                bundledata.tube_eng_len_1 = row["length"].as<double>() * 3.280839; // 单根管子英制长度1
                bundledata.tube_thy_len_1 = row["weight"].as<double>() / weight_per_meter; // 单根管子理论长度1
                bundledata.seq_id_1 = row["flow_no"].as<int>(0); // 流水号1
            }
            else if (i==1)
            {
                bundledata.tube_no_2 = row["tube_no"].as<std::string>(); // 管号2
                bundledata.tube_act_wt_2 = row["weight"].as<double>()/1000.0; // 单根管子重量2(吨)
                bundledata.tube_eng_wt_2 = row["weight"].as<double>() * 2.204622; // 单根管子英制重量2
                bundledata.tube_theory_wt_2 = row["length"].as<double>() * weight_per_meter; // 单根管子理论重量2
                bundledata.tube_len_2 = row["length"].as<double>(); // 单根管子长度2
                bundledata.tube_eng_len_2 = row["length"].as<double>() * 3.280839; // 单根管子英制长度2
                bundledata.tube_thy_len_2 = row["weight"].as<double>() / weight_per_meter; // 单根管子理论长度2
                bundledata.seq_id_2 = row["flow_no"].as<int>(0); // 流水号2
            }
            else if (i==2)
            {
                bundledata.tube_no_3 = row["tube_no"].as<std::string>(); // 管号3
                bundledata.tube_act_wt_3 = row["weight"].as<double>()/1000.0; // 单根管子重量3(吨)
                bundledata.tube_eng_wt_3 = row["weight"].as<double>() * 2.204622; // 单根管子英制重量3
                bundledata.tube_theory_wt_3 = row["length"].as<double>() * weight_per_meter; // 单根管子理论重量3
                bundledata.tube_len_3 = row["length"].as<double>(); // 单根管子长度3
                bundledata.tube_eng_len_3 = row["length"].as<double>() * 3.280839; // 单根管子英制长度3
                bundledata.tube_thy_len_3 = row["weight"].as<double>() / weight_per_meter; // 单根管子理论长度3
                bundledata.seq_id_3 = row["flow_no"].as<int>(0); // 流水号3
            }
            else if (i==3)
            {
                bundledata.tube_no_4 = row["tube_no"].as<std::string>(); // 管号4
                bundledata.tube_act_wt_4 = row["weight"].as<double>()/1000.0; // 单根管子重量4(吨)
                bundledata.tube_eng_wt_4 = row["weight"].as<double>() * 2.204622; // 单根管子英制重量4
                bundledata.tube_theory_wt_4 = row["length"].as<double>() * weight_per_meter; // 单根管子理论重量4
                bundledata.tube_len_4 = row["length"].as<double>(); // 单根管子长度4
                bundledata.tube_eng_len_4 = row["length"].as<double>() * 3.280839; // 单根管子英制长度4
                bundledata.tube_thy_len_4 = row["weight"].as<double>() / weight_per_meter; // 单根管子理论长度4
                bundledata.seq_id_4 = row["flow_no"].as<int>(0); // 流水号4 
            }
            else if (i==4)
            {
                bundledata.tube_no_5 = row["tube_no"].as<std::string>(); // 管号5
                bundledata.tube_act_wt_5 = row["weight"].as<double>()/1000.0; // 单根管子重量5(吨)
                bundledata.tube_eng_wt_5 = row["weight"].as<double>() * 2.204622; // 单根管子英制重量5
                bundledata.tube_theory_wt_5 = row["length"].as<double>() * weight_per_meter; // 单根管子理论重量5
                bundledata.tube_len_5 = row["length"].as<double>(); // 单根管子长度5
                bundledata.tube_eng_len_5 = row["length"].as<double>() * 3.280839; // 单根管子英制长度5
                bundledata.tube_thy_len_5 = row["weight"].as<double>() / weight_per_meter; // 单根管子理论长度5
                bundledata.seq_id_5 = row["flow_no"].as<int>(0); // 流水号5
            }
            else if (i==5)
            {
                bundledata.tube_no_6 = row["tube_no"].as<std::string>(); // 管号6
                bundledata.tube_act_wt_6 = row["weight"].as<double>()/1000.0; // 单根管子重量6(吨)
                bundledata.tube_eng_wt_6 = row["weight"].as<double>() * 2.204622; // 单根管子英制重量6
                bundledata.tube_theory_wt_6 = row["length"].as<double>() * weight_per_meter; // 单根管子理论重量6
                bundledata.tube_len_6 = row["length"].as<double>(); // 单根管子长度6
                bundledata.tube_eng_len_6 = row["length"].as<double>() * 3.280839; // 单根管子英制长度6
                bundledata.tube_thy_len_6 = row["weight"].as<double>() / weight_per_meter; // 单根管子理论长度6
                bundledata.seq_id_6 = row["flow_no"].as<int>(0); // 流水号6
            }
            else if (i==6)
            {
                bundledata.tube_no_7 = row["tube_no"].as<std::string>(); // 管号7
                bundledata.tube_act_wt_7 = row["weight"].as<double>()/1000.0; // 单根管子重量7(吨)
                bundledata.tube_eng_wt_7 = row["weight"].as<double>() * 2.204622; // 单根管子英制重量7
                bundledata.tube_theory_wt_7 = row["length"].as<double>() * weight_per_meter; // 单根管子理论重量7
                bundledata.tube_len_7 = row["length"].as<double>(); // 单根管子长度7
                bundledata.tube_eng_len_7 = row["length"].as<double>() * 3.280839; // 单根管子英制长度7
                bundledata.tube_thy_len_7 = row["weight"].as<double>() / weight_per_meter; // 单根管子理论长度7
                bundledata.seq_id_7 = row["flow_no"].as<int>(0); // 流水号7
            }
            else if (i==7)
            {
                bundledata.tube_no_8 = row["tube_no"].as<std::string>(); // 管号8
                bundledata.tube_act_wt_8 = row["weight"].as<double>()/1000.0; // 单根管子重量8(吨)
                bundledata.tube_eng_wt_8 = row["weight"].as<double>() * 2.204622; // 单根管子英制重量8
                bundledata.tube_theory_wt_8 = row["length"].as<double>() * weight_per_meter; // 单根管子理论重量8
                bundledata.tube_len_8 = row["length"].as<double>(); // 单根管子长度8
                bundledata.tube_eng_len_8 = row["length"].as<double>() * 3.280839; // 单根管子英制长度8
                bundledata.tube_thy_len_8 = row["weight"].as<double>() / weight_per_meter; // 单根管子理论长度8
                bundledata.seq_id_8 = row["flow_no"].as<int>(0); // 流水号8
            }
            else if (i==8)
            {
                bundledata.tube_no_9 = row["tube_no"].as<std::string>(); // 管号9
                bundledata.tube_act_wt_9 = row["weight"].as<double>()/1000.0; // 单根管子重量9(吨)
                bundledata.tube_eng_wt_9 = row["weight"].as<double>() * 2.204622; // 单根管子英制重量9
                bundledata.tube_theory_wt_9 = row["length"].as<double>() * weight_per_meter; // 单根管子理论重量9
                bundledata.tube_len_9 = row["length"].as<double>(); // 单根管子长度9
                bundledata.tube_eng_len_9 = row["length"].as<double>() * 3.280839; // 单根管子英制长度9
                bundledata.tube_thy_len_9 = row["weight"].as<double>() / weight_per_meter; // 单根管子理论长度9
                bundledata.seq_id_9 = row["flow_no"].as<int>(0); // 流水号9
            }
            else if (i==9)
            {
                bundledata.tube_no_10 = row["tube_no"].as<std::string>(); // 管号10
                bundledata.tube_act_wt_10 = row["weight"].as<double>()/1000.0; // 单根管子重量10(吨)
                bundledata.tube_eng_wt_10 = row["weight"].as<double>() * 2.204622; // 单根管子英制重量10
                bundledata.tube_theory_wt_10 = row["length"].as<double>() * weight_per_meter; // 单根管子理论重量10
                bundledata.tube_len_10 = row["length"].as<double>(); // 单根管子长度10
                bundledata.tube_eng_len_10 = row["length"].as<double>() * 3.280839; // 单根管子英制长度10
                bundledata.tube_thy_len_10 = row["weight"].as<double>() / weight_per_meter; // 单根管子理论长度10
                bundledata.seq_id_10 = row["flow_no"].as<int>(0); // 流水号10
            }
            else if (i==10)
            {
                bundledata.tube_no_11 = row["tube_no"].as<std::string>(); // 管号11
                bundledata.tube_act_wt_11 = row["weight"].as<double>()/1000.0; // 单根管子重量11(吨)
                bundledata.tube_eng_wt_11 = row["weight"].as<double>() * 2.204622; // 单根管子英制重量11
                bundledata.tube_theory_wt_11 = row["length"].as<double>() * weight_per_meter; // 单根管子理论重量11
                bundledata.tube_len_11 = row["length"].as<double>(); // 单根管子长度11
                bundledata.tube_eng_len_11 = row["length"].as<double>() * 3.280839; // 单根管子英制长度11
                bundledata.tube_thy_len_11 = row["weight"].as<double>() / weight_per_meter; // 单根管子理论长度11
                bundledata.seq_id_11 = row["flow_no"].as<int>(0); // 流水号11
            }
            else if (i==11)
            {
                bundledata.tube_no_12 = row["tube_no"].as<std::string>(); // 管号12
                bundledata.tube_act_wt_12 = row["weight"].as<double>()/1000.0; // 单根管子重量12(吨)
                bundledata.tube_eng_wt_12 = row["weight"].as<double>() * 2.204622; // 单根管子英制重量12
                bundledata.tube_theory_wt_12 = row["length"].as<double>() * weight_per_meter; // 单根管子理论重量12
                bundledata.tube_len_12 = row["length"].as<double>(); // 单根管子长度12
                bundledata.tube_eng_len_12 = row["length"].as<double>() * 3.280839; // 单根管子英制长度12
                bundledata.tube_thy_len_12 = row["weight"].as<double>() / weight_per_meter; // 单根管子理论长度12
                bundledata.seq_id_12 = row["flow_no"].as<int>(0); // 流水号12
            }
            else if (i==12)
            {
                bundledata.tube_no_13 = row["tube_no"].as<std::string>(); // 管号13
                bundledata.tube_act_wt_13 = row["weight"].as<double>()/1000.0; // 单根管子重量13(吨)
                bundledata.tube_eng_wt_13 = row["weight"].as<double>() * 2.204622; // 单根管子英制重量13
                bundledata.tube_theory_wt_13 = row["length"].as<double>() * weight_per_meter; // 单根管子理论重量13
                bundledata.tube_len_13 = row["length"].as<double>(); // 单根管子长度13
                bundledata.tube_eng_len_13 = row["length"].as<double>() * 3.280839; // 单根管子英制长度13
                bundledata.tube_thy_len_13 = row["weight"].as<double>() / weight_per_meter; // 单根管子理论长度13
                bundledata.seq_id_13 = row["flow_no"].as<int>(0); // 流水号13
            }
            else if (i==13)
            {
                bundledata.tube_no_14 = row["tube_no"].as<std::string>(); // 管号14
                bundledata.tube_act_wt_14 = row["weight"].as<double>()/1000.0; // 单根管子重量14(吨)
                bundledata.tube_eng_wt_14 = row["weight"].as<double>() * 2.204622; // 单根管子英制重量14
                bundledata.tube_theory_wt_14 = row["length"].as<double>() * weight_per_meter; // 单根管子理论重量14
                bundledata.tube_len_14 = row["length"].as<double>(); // 单根管子长度14
                bundledata.tube_eng_len_14 = row["length"].as<double>() * 3.280839; // 单根管子英制长度14
                bundledata.tube_thy_len_14 = row["weight"].as<double>() / weight_per_meter; // 单根管子理论长度14
                bundledata.seq_id_14 = row["flow_no"].as<int>(0); // 流水号14
            }
            else if (i==14)
            {
                bundledata.tube_no_15 = row["tube_no"].as<std::string>(); // 管号15
                bundledata.tube_act_wt_15 = row["weight"].as<double>()/1000.0; // 单根管子重量15(吨)
                bundledata.tube_eng_wt_15 = row["weight"].as<double>() * 2.204622; // 单根管子英制重量15
                bundledata.tube_theory_wt_15 = row["length"].as<double>() * weight_per_meter; // 单根管子理论重量15
                bundledata.tube_len_15 = row["length"].as<double>(); // 单根管子长度15
                bundledata.tube_eng_len_15 = row["length"].as<double>() * 3.280839; // 单根管子英制长度15
                bundledata.tube_thy_len_15 = row["weight"].as<double>() / weight_per_meter; // 单根管子理论长度15
                bundledata.seq_id_15 = row["flow_no"].as<int>(0); // 流水号15
            }
        }
    }
    catch (const std::exception &e)
    {
        spdlog::error("操作api_bundle_data_t失败， {}", e.what());
        return;
    }

    unsigned int error = 0;
    writeb(ctx.gplatConn, "MMWE_TQMTIWEA13", &bundledata, sizeof(bundledata), &error);
    if (error != 0)
    {
        spdlog::error("写入api_bundle_data_t失败，错误码: {}", error);
    }
}