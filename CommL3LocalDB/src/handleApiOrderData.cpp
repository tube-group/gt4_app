#include "logging.h"
#include "higplat.h"
#include "CommL3Context.h"
#include "user_types.h"
#include <ctime>

void handleApiOrderData(CommL3Context &ctx, const char *value)
{
    // 解析API_ORDER_DATA_T的值
    OrderData orderdata = read_value<OrderData>(value);

    spdlog::info("Processing API_ORDER_DATA_T: order_no={}", orderdata.order_no.c_str());

    // 将数据存储到数据库
    // 首先查询数据库中是否已经存在该合同号
    bool exists = false;
    try {
        pqxx::work txn(*ctx.pgConn);
        pqxx::result r = txn.exec(
            "SELECT COUNT(*) FROM api_order_data_t WHERE order_no = $1",
            pqxx::params{orderdata.order_no.to_string()});
        if (r[0][0].as<int>() > 0) {
            spdlog::info("Order data already exists in database: order_no={}",
                         orderdata.order_no.to_string());
            exists = true;
        }
    } catch (const std::exception &e) {
        spdlog::error("Error checking order data existence: {}", e.what());
        return;
    }

    //获取当前时间的yyyymmddhhmmss格式字符串
    std::string current_time_str;
    {
        time_t now = time(nullptr);
        struct tm tstruct;
        char buf[20];
        localtime_r(&now, &tstruct);
        strftime(buf, sizeof(buf), "%Y%m%d%H%M%S", &tstruct);
        current_time_str = buf;
    }

    if (exists) {
        // 如果合同数据已存在，选择更新
        try {
            pqxx::work txn(*ctx.pgConn);
            txn.exec(
                "UPDATE api_order_data_t SET "
                "roll_no = $1, diameter = $2, wall_thickness = $3, prod_code = $4, prod_cname = $5, "
                "heat_treat_code = $6, heat_treat_text = $7, std_sg_code = $8, std_text = $9, sg_text = $10, "
                "mat_no = $11, mat_text = $12, thread_type_code = $13, thread_type_sign = $14, end_type_code = $15, "
                "end_type_sign = $16, coupling_type_code = $17, coupling_type_sign = $18, "
                "thread_face_treat_mode_code = $19, thread_face_treat_mode = $20, length_from = $21, "
                "length_to = $22, order_unit_code = $23, order_unit = $24, order_qty = $25, order_tube = $26, "
                "order_weight = $27, fixed_order_weight = $28, unfixed_order_weight = $29, "
                "delivery_tolerance_code = $30, delivery_tolerance_unit = $31, delivery_tolerance_from = $32, "
                "delivery_tolerance_to = $33, short_rate = $34, short_from = $35, short_to = $36, "
                "single_bundle_weight_max = $37, single_bundle_tube_max = $38, oil_code = $39, oil_type = $40, "
                "stamp_req = $41, stencil_req = $42, label_req_1 = $43, label_req_2 = $44, label_req_3 = $45, "
                "label_req_4 = $46, label_req_5 = $47, label_req_6 = $48, label_req_7 = $49, label_req_8 = $50, "
                "qual_special_req = $51, produce_special_req = $52, std_pressure_mpa = $53, std_pressure_psi = $54, "
                "stabilivolt_time_min = $55, anneal_flag = $56, weight_per_meter = $57, weight_ew = $58, "
                "theory_weight_eng = $59, order_no_old = $60, color_circle = $61, color_circle_pos = $62, item_no = $63, toc = $64 "
                "WHERE order_no = $65",
                pqxx::params{orderdata.roll_no.to_string(),
                             orderdata.diameter,
                             orderdata.wall_thickness,
                             orderdata.prod_code.to_string(),
                             orderdata.prod_cname.to_string(),
                             orderdata.heat_treat_code.to_string(),
                             orderdata.heat_treat_text.to_string(),
                             orderdata.std_sg_code.to_string(),
                             orderdata.std_text.to_string(),
                             orderdata.sg_text.to_string(),
                             orderdata.mat_no.to_string(),
                             orderdata.mat_text.to_string(),
                             orderdata.thread_type_code.to_string(),
                             orderdata.thread_type_sign.to_string(),
                             orderdata.end_type_code.to_string(),
                             orderdata.end_type_sign.to_string(),
                             orderdata.coupling_type_code.to_string(),
                             orderdata.coupling_type_sign.to_string(),
                             orderdata.thread_face_treat_mode_code.to_string(),
                             orderdata.thread_face_treat_mode.to_string(),
                             orderdata.length_from,
                             orderdata.length_to,
                             orderdata.order_unit_code.to_string(),
                             orderdata.order_unit.to_string(),
                             orderdata.order_qty,
                             orderdata.order_tube,
                             orderdata.order_weight,
                             orderdata.fixed_order_weight,
                             orderdata.unfixed_order_weight,
                             orderdata.delivery_tolerance_code.to_string(),
                             orderdata.delivery_tolerance_unit.to_string(),
                             orderdata.delivery_tolerance_from,
                             orderdata.delivery_tolerance_to,
                             orderdata.short_rate,
                             orderdata.short_from,
                             orderdata.short_to,
                             orderdata.single_bundle_weight_max,
                             orderdata.single_bundle_tube_max,
                             orderdata.oil_code.to_string(),
                             orderdata.oil_type.to_string(),
                             orderdata.stamp_req.to_string(),
                             orderdata.stencil_req.to_string(),
                             orderdata.label_req_1.to_string(),
                             orderdata.label_req_2.to_string(),
                             orderdata.label_req_3.to_string(),
                             orderdata.label_req_4.to_string(),
                             orderdata.label_req_5.to_string(),
                             orderdata.label_req_6.to_string(),
                             orderdata.label_req_7.to_string(),
                             orderdata.label_req_8.to_string(),
                             orderdata.qual_special_req.to_string(),
                             orderdata.produce_special_req.to_string(),
                             orderdata.std_pressure_mpa,
                             orderdata.std_pressure_psi,
                             orderdata.stabilivolt_time_min,
                             orderdata.anneal_flag.to_string(),
                             orderdata.weight_per_meter,
                             orderdata.weight_ew,
                             orderdata.theory_weight_eng,
                             orderdata.order_no_old.to_string(),
                             orderdata.color_circle.to_string(),
                             orderdata.color_circle_pos.to_string(),
                             orderdata.item_no.to_string(),
                             current_time_str,
                             orderdata.order_no.to_string()});

            txn.commit();
            spdlog::info("Order data updated in database: order_no={}, item_no={}", orderdata.order_no.c_str(), orderdata.item_no.c_str());

            //通知前台合同数据已更新
            ctx.redis->set("REQUEST_ORDER_RESULT", "UPDATED");
            ctx.redis->publish("RealDataChanged", "REQUEST_ORDER_RESULT");
        } catch (const std::exception &e) {
            spdlog::error("Error updating order data: {}, order_no={}", e.what(), orderdata.order_no.c_str());
        }
    }
    else
    {
        // 如果合同数据不存在，插入新数据
        try {
            pqxx::work txn(*ctx.pgConn);
            txn.exec(
                "INSERT INTO api_order_data_t ("
                "order_no, item_no, roll_no, diameter, wall_thickness, prod_code, prod_cname, heat_treat_code, "
                "heat_treat_text, std_sg_code, std_text, sg_text, mat_no, mat_text, thread_type_code, "
                "thread_type_sign, end_type_code, end_type_sign, coupling_type_code, coupling_type_sign, "
                "thread_face_treat_mode_code, thread_face_treat_mode, length_from, length_to, order_unit_code, "
                "order_unit, order_qty, order_tube, order_weight, fixed_order_weight, unfixed_order_weight, "
                "delivery_tolerance_code, delivery_tolerance_unit, delivery_tolerance_from, delivery_tolerance_to, "
                "short_rate, short_from, short_to, single_bundle_weight_max, single_bundle_tube_max, oil_code, "
                "oil_type, stamp_req, stencil_req, label_req_1, label_req_2, label_req_3, label_req_4, "
                "label_req_5, label_req_6, label_req_7, label_req_8, qual_special_req, produce_special_req, "
                "std_pressure_mpa, std_pressure_psi, stabilivolt_time_min, anneal_flag, weight_per_meter, weight_ew, "
                "theory_weight_eng, order_no_old, color_circle, color_circle_pos, label_req_1_manual, label_req_2_manual, "
                "label_req_3_manual, label_req_4_manual, label_req_5_manual, label_req_6_manual, label_req_7_manual, label_req_8_manual, stencil_req_manual, toc) "
                "VALUES ("
                "$1, $2, $3, $4, $5, $6, $7, $8, $9, $10, $11, $12, $13, $14, $15, $16, $17, $18, $19, $20, "
                "$21, $22, $23, $24, $25, $26, $27, $28, $29, $30, $31, $32, $33, $34, $35, $36, $37, $38, "
                "$39, $40, $41, $42, $43, $44, $45, $46, $47, $48, $49, $50, $51, $52, $53, $54, $55, $56, "
                "$57, $58, $59, $60, $61, $62, $63, $64, $65, $66, $67, $68, $69, $70, $71, $72, $73, $74)",
                pqxx::params{orderdata.order_no.to_string(),
                             orderdata.item_no.to_string(),
                             orderdata.roll_no.to_string(),
                             orderdata.diameter,
                             orderdata.wall_thickness,
                             orderdata.prod_code.to_string(),
                             orderdata.prod_cname.to_string(),
                             orderdata.heat_treat_code.to_string(),
                             orderdata.heat_treat_text.to_string(),
                             orderdata.std_sg_code.to_string(),
                             orderdata.std_text.to_string(),
                             orderdata.sg_text.to_string(),
                             orderdata.mat_no.to_string(),
                             orderdata.mat_text.to_string(),
                             orderdata.thread_type_code.to_string(),
                             orderdata.thread_type_sign.to_string(),
                             orderdata.end_type_code.to_string(),
                             orderdata.end_type_sign.to_string(),
                             orderdata.coupling_type_code.to_string(),
                             orderdata.coupling_type_sign.to_string(),
                             orderdata.thread_face_treat_mode_code.to_string(),
                             orderdata.thread_face_treat_mode.to_string(),
                             orderdata.length_from,
                             orderdata.length_to,
                             orderdata.order_unit_code.to_string(),
                             orderdata.order_unit.to_string(),
                             orderdata.order_qty,
                             orderdata.order_tube,
                             orderdata.order_weight,
                             orderdata.fixed_order_weight,
                             orderdata.unfixed_order_weight,
                             orderdata.delivery_tolerance_code.to_string(),
                             orderdata.delivery_tolerance_unit.to_string(),
                             orderdata.delivery_tolerance_from,
                             orderdata.delivery_tolerance_to,
                             orderdata.short_rate,
                             orderdata.short_from,
                             orderdata.short_to,
                             orderdata.single_bundle_weight_max,
                             orderdata.single_bundle_tube_max,
                             orderdata.oil_code.to_string(),
                             orderdata.oil_type.to_string(),
                             orderdata.stamp_req.to_string(),
                             orderdata.stencil_req.to_string(),
                             orderdata.label_req_1.to_string(),
                             orderdata.label_req_2.to_string(),
                             orderdata.label_req_3.to_string(),
                             orderdata.label_req_4.to_string(),
                             orderdata.label_req_5.to_string(),
                             orderdata.label_req_6.to_string(),
                             orderdata.label_req_7.to_string(),
                             orderdata.label_req_8.to_string(),
                             orderdata.qual_special_req.to_string(),
                             orderdata.produce_special_req.to_string(),
                             orderdata.std_pressure_mpa,
                             orderdata.std_pressure_psi,
                             orderdata.stabilivolt_time_min,
                             orderdata.anneal_flag.to_string(),
                             orderdata.weight_per_meter,
                             orderdata.weight_ew,
                             orderdata.theory_weight_eng,
                             orderdata.order_no_old.to_string(),
                             orderdata.color_circle.to_string(),
                             orderdata.color_circle_pos.to_string(),
                             orderdata.label_req_1.to_string(),
                             orderdata.label_req_2.to_string(),
                             orderdata.label_req_3.to_string(),
                             orderdata.label_req_4.to_string(),
                             orderdata.label_req_5.to_string(),
                             orderdata.label_req_6.to_string(),
                             orderdata.label_req_7.to_string(),
                             orderdata.label_req_8.to_string(),
                             orderdata.stencil_req.to_string(),
                             current_time_str});
            txn.commit();
            spdlog::info("Order data inserted into database: order_no={}, item_no={}",
                         orderdata.order_no.c_str(), orderdata.item_no.c_str());

            //通知前台合同数据已插入
            ctx.redis->set("REQUEST_ORDER_RESULT", "INSERTED");
            ctx.redis->publish("RealDataChanged", "REQUEST_ORDER_RESULT");
        } catch (const std::exception &e) {
            spdlog::error("Error inserting order data: {}", e.what());
        }
    }
}