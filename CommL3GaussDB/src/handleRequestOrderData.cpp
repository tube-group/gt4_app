#include "logging.h"
#include "higplat.h"
#include "CommL3Context.h"
#include "user_types.h"

namespace
{
    int readWholeNumberAsInt(const GaussDB::ResultSet::Row &row, std::string_view columnName)
    {
        const double value = row.getDouble(columnName);
        if (value < static_cast<double>(std::numeric_limits<int>::min()) ||
            value > static_cast<double>(std::numeric_limits<int>::max()))
        {
            throw std::runtime_error(std::string("结果集字段值超出 INT4 范围: ") + row.getValue(columnName));
        }

        double integralPart = 0.0;
        if (std::modf(value, &integralPart) != 0.0)
        {
            throw std::runtime_error(std::string("结果集字段值包含小数，无法转换为 INT4: ") + row.getValue(columnName));
        }

        return static_cast<int>(integralPart);
    }

    std::string truncateUtf8ToBytes(const std::string &value, std::size_t maxBytes)
    {
        if (value.size() <= maxBytes)
        {
            return value;
        }

        std::size_t end = 0;
        while (end < value.size() && end < maxBytes)
        {
            const unsigned char lead = static_cast<unsigned char>(value[end]);
            std::size_t charLength = 1;

            if ((lead & 0x80u) == 0)
            {
                charLength = 1;
            }
            else if ((lead & 0xE0u) == 0xC0u)
            {
                charLength = 2;
            }
            else if ((lead & 0xF0u) == 0xE0u)
            {
                charLength = 3;
            }
            else if ((lead & 0xF8u) == 0xF0u)
            {
                charLength = 4;
            }
            else
            {
                break;
            }

            if (end + charLength > value.size() || end + charLength > maxBytes)
            {
                break;
            }

            bool validContinuation = true;
            for (std::size_t i = 1; i < charLength; ++i)
            {
                const unsigned char continuation = static_cast<unsigned char>(value[end + i]);
                if ((continuation & 0xC0u) != 0x80u)
                {
                    validContinuation = false;
                    break;
                }
            }

            if (!validContinuation)
            {
                break;
            }

            end += charLength;
        }

        return value.substr(0, end);
    }
}

bool getOrderDataFromGaussDB(CommL3Context &ctx, const std::string &orderNo, const std::string &itemNo, OrderData &orderData);

void handleREQUEST_ORDER_DATA_CMD(CommL3Context &ctx, const char *value)
{
    RequestOrderDataCmd cmd = read_value<RequestOrderDataCmd>(value);
    spdlog::info("Received REQUEST_ORDER_DATA_CMD: order_no={}, item_no={}", cmd.order_no.c_str(), cmd.item_no.c_str());
    // 查询数据库或其他数据源，获取订单数据
    // 这里假设我们查询到了一些数据，并将其发送回gPlat
    OrderData orderData;
    if (getOrderDataFromGaussDB(ctx, cmd.order_no.to_string(), cmd.item_no.to_string(), orderData))
    {
        unsigned int error;
        writeb(ctx.gplatConn, "API_ORDER_DATA_T", &orderData, sizeof(orderData), &error);
        spdlog::info("Sent ORDER_DATA_RESPONSE for order_no={}, item_no={}", cmd.order_no.c_str(), cmd.item_no.c_str());
    }
}

bool getOrderDataFromGaussDB(CommL3Context &ctx, const std::string &orderNo, const std::string &itemNo, OrderData &orderData)
{
    if (!ctx.gaussConn)
    {
        spdlog::error("GaussDB connection is not initialized");
        return false;
    }

    try
    {
        std::string query1 = "SELECT * FROM ompo.tom01 WHERE order_no = '" + orderNo + "'";
        auto result1 = ctx.gaussConn->execute(query1);

        std::string query2 = "SELECT * FROM ompo.tom01tr WHERE order_no = '" + orderNo + "'";
        auto result2 = ctx.gaussConn->execute(query2);

        std::string query3 = "select wt_ew from qmto.tqmtotwh5 WHERE order_no = '" + orderNo + "'";
        auto result3 = ctx.gaussConn->execute(query3);

        // mark
        // if (result1.getRowCount() > 0 && result2.getRowCount() > 0 && result3.getRowCount() > 0)
        if (result1.getRowCount() > 0 && result2.getRowCount() > 0)
        {
            // 我们只取第一行数据
            auto row1 = result1.getRow(0);
            auto row2 = result2.getRow(0);
            // auto row3 = result3.getRow(0);
            orderData.order_no = row1.getString("order_no");
            orderData.item_no = row2.getString("order_item_no");
            orderData.roll_no = row2.getString("current_rl_no");// 轧批号
            orderData.diameter = row2.getDouble("change_outer_dia");// 外径
            orderData.wall_thickness = row2.getDouble("change_wal_thick");// 壁厚
            orderData.prod_code = row1.getString("prod_code");// 品名细分类代码
            orderData.prod_cname = row1.getString("prod_cname");// 品名细分类中文
            orderData.heat_treat_code = row2.getString("hot_treat_method_code");// 热处理代码
            orderData.heat_treat_text = row2.getString("hot_treat_method");// 热处理方式正文
            orderData.std_sg_code = row1.getString("std_sg_code");// 标准钢级代码/标准牌号(钢级)代码
            orderData.std_text = row1.getString("sg_std");// 标准钢级正文/牌号标准
            orderData.sg_text = row1.getString("sg_sign");// 钢级正文/牌号（钢级）
            orderData.mat_no = row2.getString("mtrl_no");// 材质号
            orderData.mat_text = row2.getString("mtrl_text");// 材质正文
            orderData.thread_type_code = row2.getString("thread_type_code");// 螺纹类型代码
            orderData.thread_type_sign = row2.getString("thread_type_sign");// 螺纹类型符号
            orderData.end_type_code = row2.getString("pipeend_type_code");// 管端类型代码（管端型式代码）
            orderData.end_type_sign = row2.getString("pipeend_type_sign");// 管端类型符号（管端型式符号）
            orderData.coupling_type_code = row2.getString("coup_type_code");// 接箍类型代码
            orderData.coupling_type_sign = row2.getString("coup_type_sign");// 接箍类型符号
            orderData.thread_face_treat_mode_code = row2.getString("thread_face_treat_mode_code");// 螺纹表面处理方式代码
            orderData.thread_face_treat_mode = row2.getString("thread_face_treat_mode");// 螺纹表面处理方式
            orderData.length_from = row1.getDouble("order_len_min");// 订货长度起/订货长度下限
            orderData.length_to = row1.getDouble("order_len_max");// 订货长度止/订货长度上限
            orderData.order_unit_code = row1.getString("order_unit_code");// 订货计量单位代码0
            orderData.order_unit = "";// 订货计量单位
            orderData.order_qty = row1.getDouble("order_qty");// 订货数量
            orderData.order_tube = row1.getInt("order_tube");// 订货根数
            orderData.order_weight = row1.getDouble("order_wt");// 订货重量
            orderData.fixed_order_weight = row2.getDouble("order_fix_wt");// 定尺订货重量（订货定尺重量）
            orderData.unfixed_order_weight = row2.getDouble("order_unfix_wt");// 非定尺订货重量（订货非定尺重量）
            orderData.delivery_tolerance_code = row1.getString("delivy_tol_unit_code");// 提货公差单位代码/交货公差单位代码
            orderData.delivery_tolerance_unit = "";// 提货公差单位
            orderData.delivery_tolerance_from = static_cast<int>(row1.getDouble("delivy_tol_min"));// 提货公差起/交货公差下限
            orderData.delivery_tolerance_to = static_cast<int>(row1.getDouble("delivy_tol_max"));// 提货公差至/交货公差上限
            orderData.short_rate = static_cast<int>(row1.getDouble("order_short_rate"));// 短尺率
            orderData.short_from = row1.getDouble("order_short_min");// 短尺长度起/订货短尺长度下限
            orderData.short_to = row1.getDouble("order_short_max");// 短尺长度至/订货短尺长度上限
            // orderData.single_bundle_weight_max = readWholeNumberAsInt(row2, "single_bundle_wt_max");// 最大单捆重量(单捆最大重量)
            orderData.single_bundle_weight_max = static_cast<int>(row2.getDouble("single_bundle_wt_max"));// 最大单捆重量(单捆最大重量)          
            orderData.single_bundle_tube_max = static_cast<int>(row2.getDouble("single_bundle_tube_max"));// 最大单捆根数(单捆最大根数)
            orderData.oil_code = row2.getString("oil_kind_code");// 涂油代码（涂油种类代码）
            orderData.oil_type = row2.getString("oil_kind");// 涂油正文（涂油种类）
            orderData.stamp_req = row2.getString("stamp_req");// 压印要求
            orderData.stencil_req = row2.getString("stencil_req");// 喷印要求
            orderData.label_req_1 = row2.getString("lable_req_1");// 标签要求1
            orderData.label_req_2 = row2.getString("lable_req_2");// 标签要求2
            orderData.label_req_3 = row2.getString("lable_req_3");// 标签要求3
            orderData.label_req_4 = row2.getString("lable_req_4");// 标签要求4
            orderData.label_req_5 = row2.getString("lable_req_5");// 标签要求5
            orderData.label_req_6 = row2.getString("lable_req_6");// 标签要求6
            orderData.label_req_7 = row2.getString("lable_req_7");// 标签要求7
            orderData.label_req_8 = row2.getString("lable_req_8");// 标签要求8
            orderData.qual_special_req = "";// 质量特殊要求
            // orderData.produce_special_req = "中文";// 生产特殊要求
            orderData.produce_special_req = truncateUtf8ToBytes(
                row2.getString("produce_special_req"),
                decltype(orderData.produce_special_req)::capacity());// 生产特殊要求
            orderData.std_pressure_mpa = 0;// 标准水压压力（MPA)
            orderData.std_pressure_psi = 0;// 标准水压压力 (PSI)
            orderData.stabilivolt_time_min = 0;// 最小稳压时间
            orderData.anneal_flag = "";// 退火标志
            orderData.weight_per_meter = row2.getDouble("wt_per_meter");// 米重
            if (result3.getRowCount() > 0)
            {
                auto row3 = result3.getRow(0);
                orderData.weight_ew = row3.getDouble("wt_ew");// EW值
            }
            else
            {
                orderData.weight_ew = 0.1; // 如果没有查询到数据，设置为默认值
            }
            // orderData.weight_ew = row3.getDouble("wt_ew");// EW值
            orderData.theory_weight_eng = row2.getDouble("wt_l_eng");// 名义重量
            orderData.order_no_old = "";// 原合同号
            orderData.color_circle = "";// 色环
            orderData.color_circle_pos = "";// 色环位置

            return true;
        }
        else
        {
            spdlog::warn("No data found for order_no={}, result1.getRowCount()={}, result2.getRowCount()={}, result3.getRowCount()={}", orderNo, result1.getRowCount(), result2.getRowCount(), result3.getRowCount());
            return false;
        }
    }
    catch (const std::exception &e)
    {
        spdlog::error("Error querying GaussDB: {}", e.what());
        return false;
    }
}
// 工序表TQMTOTWH5这个表需要和tom01tr用whole_backlog和order_no 联表查询。因为工序会有多条记录。
// select b.ORDER_NO,c.CURRENT_RL_NO,b.ORDER_OUTER_DIA,b.ORDER_THICK,b.ORDER_HEIGHT,b.PROD_CODE,b.PROD_CNAME,c.HOT_TREAT_METHOD_CODE,c.HOT_TREAT_METHOD,b.STD_SG_CODE,b.SG_STD,b.SG_SIGN,
// c.MTRL_NO,c.mtrl_text,c.PIPEEND_TYPE_CODE,c.PIPEEND_TYPE_SIGN,c.PIPEEND_TYPE,c.THREAD_TYPE_CODE,c.THREAD_TYPE_SIGN,c.THREAD_TYPE,c.COUP_TYPE_CODE,c.COUP_TYPE_SIGN,c.COUP_TYPE,
// c.THREAD_FACE_TREAT_MODE_CODE,c.THREAD_FACE_TREAT_MODE,c.CTL_LEN_FROM,c.CTL_LEN_TO,b.ORDER_UNIT_CODE,b.ORDER_QTY,b.ORDER_TUBE,b.ORDER_WT,c.ORDER_FIX_WT,c.ORDER_UNFIX_WT,
// b.DELIVY_TOL_UNIT_CODE,b.DELIVY_TOL_MAX,b.DELIVY_TOL_MIN,b.ORDER_SHORT_RATE,b.ORDER_SHORT_MIN,b.ORDER_SHORT_MAX,c.SINGLE_BUNDLE_WT_MAX,c.SINGLE_BUNDLE_TUBE_MAX,c.OIL_KIND_CODE,
// c.OIL_KIND,c.STAMP_REQ,c.STENCIL_REQ,c.LABLE_REQ_1,c.LABLE_REQ_2,c.LABLE_REQ_3,c.LABLE_REQ_4,c.LABLE_REQ_5,c.LABLE_REQ_6,c.LABLE_REQ_7,c.LABLE_REQ_8,b.manu_remark,c.SPECIAL_REMARK,
// a.STD_PRESSURE_MPA,a.STD_PRESSURE_PSI,a.STBL_VOLT_TIME_MIN,c.WT_PER_METER,a.WT_EW,c.WT_L_ENG,b.mic,c.CTL_TUBE_DIA_FROM,c.CTL_TUBE_DIA_TO,c.CTL_WAL_THICK_FROM,c.CTL_WAL_THICK_TO,
// c.CTL_HEIGHT_FROM,c.CTL_HEIGHT_TO,c.LEN_GRADE_CODE,c.LEN_GRADE from qmto.tqmtotwh5 as a ,ompo.tom01 as b, ompo.tom01tr as c 
// where a.order_no = b.order_no and a.order_no = c.order_no and a.whole_backlog = c.whole_backlog
