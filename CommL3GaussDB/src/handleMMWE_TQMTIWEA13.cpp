#include "logging.h"
#include "higplat.h"
#include "CommL3Context.h"
#include "user_types.h"

bool writeBundleDataToGaussDB(CommL3Context &ctx, mmwe_tqmtiwea13_t &bundleData);

void handleMMWE_TQMTIWEA13(CommL3Context &ctx, const char *value)
{
    mmwe_tqmtiwea13_t mmwe_tqmtiwea13 = read_value<mmwe_tqmtiwea13_t>(value);
    spdlog::info("Received MMWE_TQMTIWEA13: order_no={}, order_item_no={}, mat_no={}", mmwe_tqmtiwea13.order_no.c_str(), mmwe_tqmtiwea13.order_item_no.c_str(), mmwe_tqmtiwea13.mat_no.c_str());

    if (writeBundleDataToGaussDB(ctx, mmwe_tqmtiwea13))
    {
    }
}

bool writeBundleDataToGaussDB(CommL3Context &ctx, mmwe_tqmtiwea13_t &bundleData)
{
    try
    {
        auto numericParam = [](double value)
        {
            return GaussDB::Connection::Param::numeric(std::to_string(value));
        };

        // 向表mmwe.tqmtiwea13里插入一条数据，并输出是否成功
        auto res = ctx.gaussConn->executeParams(
            "INSERT INTO mmwe.tqmtiwea13 ("
            "operator_flag, prod_date, prod_tm, prod_shift_no, prod_shift_group, prod_job_point, dest_code, "
            "order_no, order_item_no, prod_code, prod_cname, order_outer_dia, order_wal_thick, order_height, "
            "std_sg_code, sg_sign, sg_std, mtrl_no, mtrl_text, pipeend_type_code, pipeend_type_sign, pipeend_type, "
            "thread_type_code, thread_type_sign, thread_type, old_order_no, rl_no, pono, sample_lot_no, mat_no, "
            "bundle_type, mat_tube, mat_act_wt, mat_eng_wt, total_len, eng_total_len, len_from, len_to, mat_theory_wt, thy_total_len, "
            "order_end_flag, ctl_tube_dia_from, ctl_tube_dia_to, ctl_wal_thick_from, ctl_wal_thick_to, ctl_height_from, ctl_height_to, "
            "coup_pono, coup_sample_lot_no, protect_size, protect_lot_no, protect_factory, compound_size, compound_lot_no, compound_factory, "
            "coating_size, coating_lot_no, coating_factory, stock_place_no, tube_no_1, tube_act_wt_1, tube_eng_wt_1, tube_theory_wt_1, "
            "tube_len_1, tube_eng_len_1, tube_thy_len_1, seq_id_1, tube_no_2, tube_act_wt_2, tube_eng_wt_2, tube_theory_wt_2, tube_len_2, "
            "tube_eng_len_2, tube_thy_len_2, seq_id_2, tube_no_3, tube_act_wt_3, tube_eng_wt_3, tube_theory_wt_3, tube_len_3, tube_eng_len_3, "
            "tube_thy_len_3, seq_id_3, tube_no_4, tube_act_wt_4, tube_eng_wt_4, tube_theory_wt_4, tube_len_4, tube_eng_len_4, tube_thy_len_4, seq_id_4, "
            "tube_no_5, tube_act_wt_5, tube_eng_wt_5, tube_theory_wt_5, tube_len_5, tube_eng_len_5, tube_thy_len_5, seq_id_5, tube_no_6, tube_act_wt_6, "
            "tube_eng_wt_6, tube_theory_wt_6, tube_len_6, tube_eng_len_6, tube_thy_len_6, seq_id_6, tube_no_7, tube_act_wt_7, tube_eng_wt_7, tube_theory_wt_7, "
            "tube_len_7, tube_eng_len_7, tube_thy_len_7, seq_id_7, tube_no_8, tube_act_wt_8, tube_eng_wt_8, tube_theory_wt_8, tube_len_8, tube_eng_len_8, "
            "tube_thy_len_8, seq_id_8, tube_no_9, tube_act_wt_9, tube_eng_wt_9, tube_theory_wt_9, tube_len_9, tube_eng_len_9, tube_thy_len_9, seq_id_9, tube_no_10, "
            "tube_act_wt_10, tube_eng_wt_10, tube_theory_wt_10, tube_len_10, tube_eng_len_10, tube_thy_len_10, seq_id_10, tube_no_11, tube_act_wt_11, tube_eng_wt_11, "
            "tube_theory_wt_11, tube_len_11, tube_eng_len_11, tube_thy_len_11, seq_id_11, tube_no_12, tube_act_wt_12, tube_eng_wt_12, tube_theory_wt_12, tube_len_12, "
            "tube_eng_len_12, tube_thy_len_12, seq_id_12, tube_no_13, tube_act_wt_13, tube_eng_wt_13, tube_theory_wt_13, tube_len_13, tube_eng_len_13, tube_thy_len_13, "
            "seq_id_13, tube_no_14, tube_act_wt_14, tube_eng_wt_14, tube_theory_wt_14, tube_len_14, tube_eng_len_14, tube_thy_len_14, seq_id_14, tube_no_15, "
            "tube_act_wt_15, tube_eng_wt_15, tube_theory_wt_15, tube_len_15, tube_eng_len_15, tube_thy_len_15, seq_id_15"
            ") VALUES ("
            "$1, $2, $3, $4, $5, $6, $7, $8, $9, $10, $11, $12, $13, $14, $15, $16, $17, $18, $19, $20, $21, $22, $23, $24, $25, $26, $27, $28, $29, $30, "
            "$31, $32, $33, $34, $35, $36, $37, $38, $39, $40, $41, $42, $43, $44, $45, $46, $47, $48, $49, $50, $51, $52, $53, $54, $55, $56, $57, $58, $59, $60, "
            "$61, $62, $63, $64, $65, $66, $67, $68, $69, $70, $71, $72, $73, $74, $75, $76, $77, $78, $79, $80, $81, $82, $83, $84, $85, $86, $87, $88, $89, $90, "
            "$91, $92, $93, $94, $95, $96, $97, $98, $99, $100, $101, $102, $103, $104, $105, $106, $107, $108, $109, $110, $111, $112, $113, $114, $115, $116, $117, $118, $119, $120, "
            "$121, $122, $123, $124, $125, $126, $127, $128, $129, $130, $131, $132, $133, $134, $135, $136, $137, $138, $139, $140, $141, $142, $143, $144, $145, $146, $147, $148, $149, $150, "
            "$151, $152, $153, $154, $155, $156, $157, $158, $159, $160, $161, $162, $163, $164, $165, $166, $167, $168, $169, $170, $171, $172, $173, $174, $175, $176, $177, $178, $179"
            ");",
            std::vector<GaussDB::Connection::Param>{
                GaussDB::Connection::Param::text(bundleData.operator_flag.c_str()),
                GaussDB::Connection::Param::text(bundleData.prod_date.c_str()),
                GaussDB::Connection::Param::text(bundleData.prod_tm.c_str()),
                GaussDB::Connection::Param::text(bundleData.prod_shift_no.c_str()),
                GaussDB::Connection::Param::text(bundleData.prod_shift_group.c_str()),
                GaussDB::Connection::Param::text(bundleData.prod_job_point.c_str()),
                GaussDB::Connection::Param::text(bundleData.dest_code.c_str()),
                GaussDB::Connection::Param::text(bundleData.order_no.c_str()),
                GaussDB::Connection::Param::text(bundleData.order_item_no.c_str()),
                GaussDB::Connection::Param::text(bundleData.prod_code.c_str()),
                GaussDB::Connection::Param::text(bundleData.prod_cname.c_str()),
                numericParam(bundleData.order_outer_dia),
                numericParam(bundleData.order_wal_thick),
                numericParam(bundleData.order_height),
                GaussDB::Connection::Param::text(bundleData.std_sg_code.c_str()),
                GaussDB::Connection::Param::text(bundleData.sg_sign.c_str()),
                GaussDB::Connection::Param::text(bundleData.sg_std.c_str()),
                GaussDB::Connection::Param::text(bundleData.mtrl_no.c_str()),
                GaussDB::Connection::Param::text(bundleData.mtrl_text.c_str()),
                GaussDB::Connection::Param::text(bundleData.pipeend_type_code.c_str()),
                GaussDB::Connection::Param::text(bundleData.pipeend_type_sign.c_str()),
                GaussDB::Connection::Param::text(bundleData.pipeend_type.c_str()),
                GaussDB::Connection::Param::text(bundleData.thread_type_code.c_str()),
                GaussDB::Connection::Param::text(bundleData.thread_type_sign.c_str()),
                GaussDB::Connection::Param::text(bundleData.thread_type.c_str()),
                GaussDB::Connection::Param::text(bundleData.old_order_no.c_str()),
                GaussDB::Connection::Param::text(bundleData.rl_no.c_str()),
                GaussDB::Connection::Param::text(bundleData.pono.c_str()),
                GaussDB::Connection::Param::text(bundleData.sample_lot_no.c_str()),
                GaussDB::Connection::Param::text(bundleData.mat_no.c_str()),
                GaussDB::Connection::Param::text(bundleData.bundle_type.c_str()),
                GaussDB::Connection::Param::int4(bundleData.mat_tube),
                numericParam(bundleData.mat_act_wt),
                numericParam(bundleData.mat_eng_wt),
                numericParam(bundleData.total_len),
                numericParam(bundleData.eng_total_len),
                numericParam(bundleData.len_from),
                numericParam(bundleData.len_to),
                numericParam(bundleData.mat_theory_wt),
                numericParam(bundleData.thy_total_len),
                GaussDB::Connection::Param::text(bundleData.order_end_flag.c_str()),
                numericParam(bundleData.ctl_tube_dia_from),
                numericParam(bundleData.ctl_tube_dia_to),
                numericParam(bundleData.ctl_wal_thick_from),
                numericParam(bundleData.ctl_wal_thick_to),
                numericParam(bundleData.ctl_height_from),
                numericParam(bundleData.ctl_height_to),
                GaussDB::Connection::Param::text(bundleData.coup_pono.c_str()),
                GaussDB::Connection::Param::text(bundleData.coup_sample_lot_no.c_str()),
                GaussDB::Connection::Param::text(bundleData.protect_size.c_str()),
                GaussDB::Connection::Param::text(bundleData.protect_lot_no.c_str()),
                GaussDB::Connection::Param::text(bundleData.protect_factory.c_str()),
                GaussDB::Connection::Param::text(bundleData.compound_size.c_str()),
                GaussDB::Connection::Param::text(bundleData.compound_lot_no.c_str()),
                GaussDB::Connection::Param::text(bundleData.compound_factory.c_str()),
                GaussDB::Connection::Param::text(bundleData.coating_size.c_str()),
                GaussDB::Connection::Param::text(bundleData.coating_lot_no.c_str()),
                GaussDB::Connection::Param::text(bundleData.coating_factory.c_str()),
                GaussDB::Connection::Param::text(bundleData.stock_place_no.c_str()),
                GaussDB::Connection::Param::text(bundleData.tube_no_1.c_str()),
                numericParam(bundleData.tube_act_wt_1),
                numericParam(bundleData.tube_eng_wt_1),
                numericParam(bundleData.tube_theory_wt_1),
                numericParam(bundleData.tube_len_1),
                numericParam(bundleData.tube_eng_len_1),
                numericParam(bundleData.tube_thy_len_1),
                GaussDB::Connection::Param::int4(bundleData.seq_id_1),
                GaussDB::Connection::Param::text(bundleData.tube_no_2.c_str()),
                numericParam(bundleData.tube_act_wt_2),
                numericParam(bundleData.tube_eng_wt_2),
                numericParam(bundleData.tube_theory_wt_2),
                numericParam(bundleData.tube_len_2),
                numericParam(bundleData.tube_eng_len_2),
                numericParam(bundleData.tube_thy_len_2),
                GaussDB::Connection::Param::int4(bundleData.seq_id_2),
                GaussDB::Connection::Param::text(bundleData.tube_no_3.c_str()),
                numericParam(bundleData.tube_act_wt_3),
                numericParam(bundleData.tube_eng_wt_3),
                numericParam(bundleData.tube_theory_wt_3),
                numericParam(bundleData.tube_len_3),
                numericParam(bundleData.tube_eng_len_3),
                numericParam(bundleData.tube_thy_len_3),
                GaussDB::Connection::Param::int4(bundleData.seq_id_3),
                GaussDB::Connection::Param::text(bundleData.tube_no_4.c_str()),
                numericParam(bundleData.tube_act_wt_4),
                numericParam(bundleData.tube_eng_wt_4),
                numericParam(bundleData.tube_theory_wt_4),
                numericParam(bundleData.tube_len_4),
                numericParam(bundleData.tube_eng_len_4),
                numericParam(bundleData.tube_thy_len_4),
                GaussDB::Connection::Param::int4(bundleData.seq_id_4),
                GaussDB::Connection::Param::text(bundleData.tube_no_5.c_str()),
                numericParam(bundleData.tube_act_wt_5),
                numericParam(bundleData.tube_eng_wt_5),
                numericParam(bundleData.tube_theory_wt_5),
                numericParam(bundleData.tube_len_5),
                numericParam(bundleData.tube_eng_len_5),
                numericParam(bundleData.tube_thy_len_5),
                GaussDB::Connection::Param::int4(bundleData.seq_id_5),
                GaussDB::Connection::Param::text(bundleData.tube_no_6.c_str()),
                numericParam(bundleData.tube_act_wt_6),
                numericParam(bundleData.tube_eng_wt_6),
                numericParam(bundleData.tube_theory_wt_6),
                numericParam(bundleData.tube_len_6),
                numericParam(bundleData.tube_eng_len_6),
                numericParam(bundleData.tube_thy_len_6),
                GaussDB::Connection::Param::int4(bundleData.seq_id_6),
                GaussDB::Connection::Param::text(bundleData.tube_no_7.c_str()),
                numericParam(bundleData.tube_act_wt_7),
                numericParam(bundleData.tube_eng_wt_7),
                numericParam(bundleData.tube_theory_wt_7),
                numericParam(bundleData.tube_len_7),
                numericParam(bundleData.tube_eng_len_7),
                numericParam(bundleData.tube_thy_len_7),
                GaussDB::Connection::Param::int4(bundleData.seq_id_7),
                GaussDB::Connection::Param::text(bundleData.tube_no_8.c_str()),
                numericParam(bundleData.tube_act_wt_8),
                numericParam(bundleData.tube_eng_wt_8),
                numericParam(bundleData.tube_theory_wt_8),
                numericParam(bundleData.tube_len_8),
                numericParam(bundleData.tube_eng_len_8),
                numericParam(bundleData.tube_thy_len_8),
                GaussDB::Connection::Param::int4(bundleData.seq_id_8),
                GaussDB::Connection::Param::text(bundleData.tube_no_9.c_str()),
                numericParam(bundleData.tube_act_wt_9),
                numericParam(bundleData.tube_eng_wt_9),
                numericParam(bundleData.tube_theory_wt_9),
                numericParam(bundleData.tube_len_9),
                numericParam(bundleData.tube_eng_len_9),
                numericParam(bundleData.tube_thy_len_9),
                GaussDB::Connection::Param::int4(bundleData.seq_id_9),
                GaussDB::Connection::Param::text(bundleData.tube_no_10.c_str()),
                numericParam(bundleData.tube_act_wt_10),
                numericParam(bundleData.tube_eng_wt_10),
                numericParam(bundleData.tube_theory_wt_10),
                numericParam(bundleData.tube_len_10),
                numericParam(bundleData.tube_eng_len_10),
                numericParam(bundleData.tube_thy_len_10),
                GaussDB::Connection::Param::int4(bundleData.seq_id_10),
                GaussDB::Connection::Param::text(bundleData.tube_no_11.c_str()),
                numericParam(bundleData.tube_act_wt_11),
                numericParam(bundleData.tube_eng_wt_11),
                numericParam(bundleData.tube_theory_wt_11),
                numericParam(bundleData.tube_len_11),
                numericParam(bundleData.tube_eng_len_11),
                numericParam(bundleData.tube_thy_len_11),
                GaussDB::Connection::Param::int4(bundleData.seq_id_11),
                GaussDB::Connection::Param::text(bundleData.tube_no_12.c_str()),
                numericParam(bundleData.tube_act_wt_12),
                numericParam(bundleData.tube_eng_wt_12),
                numericParam(bundleData.tube_theory_wt_12),
                numericParam(bundleData.tube_len_12),
                numericParam(bundleData.tube_eng_len_12),
                numericParam(bundleData.tube_thy_len_12),
                GaussDB::Connection::Param::int4(bundleData.seq_id_12),
                GaussDB::Connection::Param::text(bundleData.tube_no_13.c_str()),
                numericParam(bundleData.tube_act_wt_13),
                numericParam(bundleData.tube_eng_wt_13),
                numericParam(bundleData.tube_theory_wt_13),
                numericParam(bundleData.tube_len_13),
                numericParam(bundleData.tube_eng_len_13),
                numericParam(bundleData.tube_thy_len_13),
                GaussDB::Connection::Param::int4(bundleData.seq_id_13),
                GaussDB::Connection::Param::text(bundleData.tube_no_14.c_str()),
                numericParam(bundleData.tube_act_wt_14),
                numericParam(bundleData.tube_eng_wt_14),
                numericParam(bundleData.tube_theory_wt_14),
                numericParam(bundleData.tube_len_14),
                numericParam(bundleData.tube_eng_len_14),
                numericParam(bundleData.tube_thy_len_14),
                GaussDB::Connection::Param::int4(bundleData.seq_id_14),
                GaussDB::Connection::Param::text(bundleData.tube_no_15.c_str()),
                numericParam(bundleData.tube_act_wt_15),
                numericParam(bundleData.tube_eng_wt_15),
                numericParam(bundleData.tube_theory_wt_15),
                numericParam(bundleData.tube_len_15),
                numericParam(bundleData.tube_eng_len_15),
                numericParam(bundleData.tube_thy_len_15),
                GaussDB::Connection::Param::int4(bundleData.seq_id_15)
            });
        return true;
    }
    catch (const std::exception &e)
    {
        spdlog::error("高斯数据库插入数据失败: {}", e.what());
        return false;
    }
}