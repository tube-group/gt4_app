// Basket.cpp 成品料筐工位实现
#include "Basket.h"
#include "TubeTrackContext.h"
#include "logging.h"
#include "cmath"
#include "user_types.h"
#include "CalculateShift.h"

void CBasket::UpdateForm()
{
    // 刷新成品料筐的界面显示
    if (m_ctx && m_ctx->redis)
    {
        m_ctx->redis->set(m_redisKey, convertToJson());
        spdlog::info("{}: {} updated", m_positionName, m_redisKey);
        // 发布详细消息到 RealDataChanged 主题
        m_ctx->redis->publish("RealDataChanged", m_redisKey);
    }
}

void CBasket::DebugOut()
{
    // 输出成品料筐管子数量
    if (m_ctx && m_ctx->redis)
    {
        spdlog::info("{} tube count: {}", m_positionName, Count());
        // spdlog::info("{}: {} = {}", m_positionName, m_redisKey, convertToJson());
    }
}

void CBasket::ReadParameterSet()
{
    try
    {
        pqxx::nontransaction ntx(*m_ctx->pgConn);
        const pqxx::result result = ntx.exec(
            "SELECT * FROM parameter_set LIMIT 1");

        if (result.empty())
        {
            spdlog::warn("parameter_set表无数据，使用默认成品料筐参数");
            return;
        }

        const auto &row = result[0];

        bundle_number_ = row["bundle_number"].as<int>();            // 打捆根数
        bundle_flow_no_ = row["bundle_flow_no"].as<int>();          // 管捆流水号
        bundle_first_type_ = row["bundle_first_type"].as<int>();    // 管捆号首位(1油管2套管）
        product_job_point_ = row["produce_job_point"].as<string>(); // 作业点代码
        direction_code_ = row["direction_code"].as<string>();       // 去向代码
        bundle_type_ = row["bundle_type"].as<string>();             // 管捆类型
        weight_packaging_ = row["weight_packaging"].as<double>(0);  // 包装材料重量
        melt_no_coupling_ = row["melt_no_coupling"].as<string>(""); // 接箍炉号
        lot_no_coupling_ = row["lot_no_coupling"].as<string>("");   // 接箍批号

        spdlog::info("成品筐工位从数据库加载生产计划参数成功");
        spdlog::info("bundle_number_ 读取结果: {}", bundle_number_);

        // 将下一根管捆的流水号写入Redis，NEXT_BUNDLE_FLOW_NO键用于前端显示
        m_ctx->redis->set("NEXT_BUNDLE_FLOW_NO", std::to_string(bundle_flow_no_));
        spdlog::info("下一管捆流水号已写入Redis: NEXT_BUNDLE_FLOW_NO={}", bundle_flow_no_);
        m_ctx->redis->publish("RealDataChanged", "NEXT_BUNDLE_FLOW_NO");

        // 将打捆根数写入Redis，BUNDLE_NUMBER键用于前端显示
        m_ctx->redis->set("BUNDLE_NUMBER", std::to_string(bundle_number_));
        spdlog::info("打捆根数已写入Redis: BUNDLE_NUMBER={}", bundle_number_);
        m_ctx->redis->publish("RealDataChanged", "BUNDLE_NUMBER");
    }
    catch (const std::exception &e)
    {
        spdlog::error("读取parameter_set失败，使用默认值: {}", e.what());
    }
}

//---------------计算打捆根数---------------
int CBasket::BundleCount()
{
    return bundle_number_;
}

//---------------判断成品料筐是否有空间放置新管子---------------
bool CBasket::HasSpace()
{
    return Count() < static_cast<size_t>(max_count_) && Count() < static_cast<size_t>(bundle_number_);
}

//---------------PushBack时触发打捆逻辑---------------
void CBasket::EntryTrigger(const CTube &tube)
{
    spdlog::info("=== EntryTrigger 被调用 ===");
    spdlog::info("当前管子数量: {}, 打捆根数: {}", Count(), bundle_number_);
    // 当成品料筐内管子数量达到打捆根数时，执行打捆操作
    if (Count() >= bundle_number_)
    {
        spdlog::info("成品料筐内管子数量 {} 已达到打捆根数 {}, 执行打捆操作", Count(), bundle_number_);
        if (Bundle())
        {
            UpdateForm();
        }
        else
        {
            spdlog::warn("成品料筐打捆失败，保留当前筐内管子，等待后续处理");
        }
    }
    else
    {
        spdlog::info("成品料筐内管子数量 {} 未达到打捆根数 {}, 继续等待", Count(), bundle_number_);
    }
}

//---------------执行打捆操作，生成管捆信息并清空成品料筐---------------
bool CBasket::Bundle()
{
    // 管捆信息初始化
    string bundleno;                                         // 管捆号
    double lengthsum = 0.0, weightsum = 0.0;                 // 打捆总长度和总重量
    double lengthmin = 100.0, lengthmax = 0.0;               // 管长极值
    int tubecount = 0, flow_no = 0;                          // 管子数量和流水号
    string order_no, melt_no, lot_no, item_no, roll_no = ""; // 订单号、炉号、试批号、项目号、轧批号

    tubecount = Count();
    if (tubecount == 0)
    {
        spdlog::warn("成品料筐内无管子，无法打捆");
        return false;
    }

    ReadParameterSet();

    // 统计管捆信息
    for (const auto &tubePtr : Tubes())
    {
        const auto &tube = *tubePtr;
        lengthsum += tube.length;
        weightsum += tube.weight;
        order_no = tube.order_no;
        melt_no = tube.melt_no;
        lot_no = tube.lot_no;
        item_no = tube.item_no;
        roll_no = tube.roll_no;
        if (tube.flow_no > flow_no)
        {
            flow_no = tube.flow_no; // 以流水号最大的管子为准
        }
        if (tube.length > lengthmax)
        {
            lengthmax = tube.length;
        }
        if (tube.length < lengthmin)
        {
            lengthmin = tube.length;
        }
        spdlog::info("统计管子: roll_no={}", tube.roll_no);
    }

    // 延迟交班逻辑
    // 1. 获取延迟交班标志ShiftDelay
    unsigned int err;
    bool ShiftDelay = false;
    bool result = readb(m_ctx->gplatConn, "SHIFT_DELAY", &ShiftDelay, sizeof(ShiftDelay), &err);
    if (!result)
    {
        spdlog::warn("读取SHIFT_DELAY失败，使用默认值: err={}", err);
        ShiftDelay = false;
    }
    // 获取当前日期时间
    struct tm t;
    GetDateTime(t);
    int tm = t.tm_hour * 10000 + t.tm_min * 100 + t.tm_sec;

    // 3. 判断是否在延迟交班有效区间内
    bool inDelayWindow = false;
    if (ShiftDelay) {
        // 早晨交接窗口：07:45 ~ 08:30
        // 傍晚交接窗口：19:45 ~ 20:30
        if ((tm >= 74500 && tm <= 83000) || (tm >= 194500 && tm <= 203000)) {
            inDelayWindow = true;
        }
    }
    // 4. 计算班次和生产时间
    int ban_ci;
    char produce_time_bundle[32];
    char produce_time_tube[32];

    if (inDelayWindow) 
    {
        // 延迟交班：按当前时间往前推3小时计算班次
        time_t rawTime = mktime(&t);
        rawTime -= 3 * 3600;  // 减3小时
        struct tm* pAdjustedTime = localtime(&rawTime);
        
        // 计算班次（用调整后的时间）
        std::string strBh;
        CalcShift(*pAdjustedTime, strBh, ban_ci);
        
        // 打捆时间固定为 07:44:00 或 19:44:00
        if (tm >= 74500 && tm <= 83000) 
        {
            // 早晨窗口：固定为 07:44:00
            strftime(produce_time_bundle, sizeof(produce_time_bundle), "%Y%m%d074400", &t);
            strftime(produce_time_tube, sizeof(produce_time_tube), "%Y-%m-%d 07:44:00", &t);
        } else 
        {
            // 傍晚窗口：固定为 19:44:00
            strftime(produce_time_bundle, sizeof(produce_time_bundle), "%Y%m%d194400", &t);
            strftime(produce_time_tube, sizeof(produce_time_tube), "%Y-%m-%d 19:44:00", &t);
        }
        
        spdlog::info("延迟交班模式：当前时间减3小时计算班次 ban_ci={}", ban_ci);

    } else 
    {
        // 正常模式：按当前时间计算班次
        readb(m_ctx->gplatConn, "SHIFT_NO", &ban_ci, sizeof(ban_ci), &err);
        
        strftime(produce_time_bundle, sizeof(produce_time_bundle), "%Y%m%d%H%M%S", &t);
        strftime(produce_time_tube, sizeof(produce_time_tube), "%Y-%m-%d %H:%M:%S", &t);
        
        if (ShiftDelay) {
            spdlog::info("延迟交班标记已开启，但当前不在有效时间窗口内，按正常逻辑处理");
        }
        spdlog::info("正常模式：按当前时间计算班次 ban_ci={}", ban_ci);
    }

    spdlog::info("生产时间: bundle表={}, tube表={}", produce_time_bundle, produce_time_tube);

    // 数据库操作
    // 1.查询合同数据————api_order_data_t
    // 使用单个事务处理所有数据库操作
    pqxx::work txn(*m_ctx->pgConn);

    // 生成管捆号：管捆号格式=首位标志(1油管2套管) + 0 + 班次 + 4位流水号
    stringstream str;
    str << bundle_first_type_ << "0";
    str << (ban_ci % 10);
    str << setw(4) << setfill('0') << bundle_flow_no_;
    bundleno = str.str();
    spdlog::info("生成管捆号: {}", bundleno);
    // 检查管捆号是否已存在
    const pqxx::result bundleExists = txn.exec(
        "SELECT 1 FROM api_bundle_data_t "
        "WHERE order_no = $1 AND item_no = $2 AND bundle_no = $3 LIMIT 1",
        pqxx::params{order_no, item_no, bundleno});

    if (!bundleExists.empty())
    {
        // 已存在
        spdlog::warn("检测到重复管捆号: order_no={}, item_no={}, bundle_no={}",
                     order_no,
                     item_no,
                     bundleno);
        return false; // 直接返回失败，避免无限循环
    }

    const pqxx::result orderResult = txn.exec(
        "SELECT weight_per_meter, weight_ew, diameter, wall_thickness, "
        "prod_code, prod_cname, mat_no, mat_text, std_sg_code, sg_text, std_text, "
        "end_type_code, end_type_sign, thread_type_code, thread_type_sign, "
        "coupling_type_code, coupling_type_sign, order_no_old, end_type, thread_type, "
        "diameter_down_ctrl, diameter_up_ctrl, wal_thick_down_ctrl, wal_thick_up_ctrl "
        "FROM api_order_data_t "
        "WHERE order_no = $1 AND item_no = $2 LIMIT 1",
        pqxx::params{order_no, item_no});

    if (orderResult.empty())
    {
        spdlog::warn("没有查到合同数据 order_no={}, item_no={}", order_no, item_no);
        return false; // 直接返回失败，避免后续操作出错
    }

    double weight_per_meter = 0.0;
    double weight_ew = 0.0;
    double diameter = 0.0;
    double wall_thickness = 0.0;
    string prod_code = "";
    string prod_cname = "";
    string mat_no = "";
    string mat_text = "";
    string std_sg_code = "";
    string sg_text = "";
    string std_text = "";
    string end_type_code = "";
    string end_type_sign = "";
    string thread_type_code = "";
    string thread_type_sign = "";
    string coupling_type_code = "";
    string coupling_type_sign = "";
    string order_no_old = "";
    string end_type = "";
    string thread_type = "";
    string diameter_down_ctrl = "";
    string diameter_up_ctrl = "";
    string wal_thick_down_ctrl = "";
    string wal_thick_up_ctrl = "";

    const auto &row = orderResult[0];
    weight_per_meter = row["weight_per_meter"].as<double>(); // 米重
    weight_ew = row["weight_ew"].as<double>();               // EW值
    diameter = row["diameter"].as<double>();                 // 外径
    wall_thickness = row["wall_thickness"].as<double>();     // 壁厚
    spdlog::info("1");
    prod_code = row["prod_code"].as<string>(); // 品名细分类代码
    spdlog::info("2");
    prod_cname = row["prod_cname"].as<string>(); // 品名细分类
    spdlog::info("3");
    mat_no = row["mat_no"].as<string>(); // 材质号
    spdlog::info("4");
    mat_text = row["mat_text"].as<string>(); // 材质正文
    spdlog::info("5");
    std_sg_code = row["std_sg_code"].as<string>(); // 标准钢级代码
    spdlog::info("6");
    sg_text = row["sg_text"].as<string>(); // 钢级正文
    spdlog::info("7");
    std_text = row["std_text"].as<string>(); // 标准正文
    spdlog::info("8");
    end_type_code = row["end_type_code"].as<string>(); // 管端类型代码
    spdlog::info("9");
    end_type_sign = row["end_type_sign"].as<string>(); // 管端类型符号
    spdlog::info("10");
    thread_type_code = row["thread_type_code"].as<string>(); // 螺纹类型代码
    spdlog::info("11");
    thread_type_sign = row["thread_type_sign"].as<string>(); // 螺纹类型符号
    spdlog::info("12");
    coupling_type_code = row["coupling_type_code"].as<string>(); // 接箍类型代码
    spdlog::info("13");
    coupling_type_sign = row["coupling_type_sign"].as<string>(); // 接箍类型符号
    spdlog::info("14");
    order_no_old = row["order_no_old"].as<string>(); // 原合同号
    spdlog::info("15");
    end_type = row["end_type"].as<string>(); // 管端类型
    spdlog::info("16");
    thread_type = row["thread_type"].as<string>(); // 螺纹类型
    spdlog::info("17");
    diameter_down_ctrl = row["diameter_down_ctrl"].as<string>(); // 外径下限_内控
    spdlog::info("18");
    diameter_up_ctrl = row["diameter_up_ctrl"].as<string>(); // 外径上限_内控
    spdlog::info("19");
    wal_thick_down_ctrl = row["wal_thick_down_ctrl"].as<string>(); // 壁厚下限_内控
    spdlog::info("20");
    wal_thick_up_ctrl = row["wal_thick_up_ctrl"].as<string>(); // 壁厚上限_内控
    spdlog::info("21");

    // 计算派生字段
    weightsum = std::round(weightsum);                                                              // 总重量取整
    double weight_eng = std::round(weightsum * 2.204622 * 1000.0) / 1000.0;                         // 英制重量
    lengthsum = std::round(lengthsum * 1000.0) / 1000.0;                                            // 保留3位小数
    double length_eng = std::round(lengthsum * 3.280839 * 1000.0) / 1000.0;                         // 英制长度
    double length_from = std::round(lengthmin * 1000.0) / 1000.0;                                   // 最短
    double length_to = std::round(lengthmax * 1000.0) / 1000.0;                                     // 最长
    int theory_weight = static_cast<int>(std::round((lengthsum * weight_per_meter) + weight_ew));   // 理论重量
    double theory_total_length = lengthsum;                                                         // 理论总长度
    int gross_weight = static_cast<int>(weightsum + weight_packaging_ / 100.0);                     // 毛重
    string ban_ci_str = std::to_string(ban_ci);                                                     // ban_ci是varchar(2)
    spdlog::info("计算管捆信息: lengthsum={}, weightsum={}, weight_eng={}, length_eng={}, length_from={}, length_to={}, theory_weight={}, gross_weight={}",
                 lengthsum, weightsum, weight_eng, length_eng, length_from, length_to, theory_weight, gross_weight);

    try
    {
        // 删除重复流水号的管子数据(按业务键: order_no + item_no + flow_no)
        size_t deleted_tube_rows = 0;
        for (const auto &tubePtr : Tubes())
        {
            const auto &tube = *tubePtr;
            const pqxx::result deleteResult = txn.exec(
                "DELETE FROM api_tube_data_t "
                "WHERE order_no = $1 AND item_no = $2 AND flow_no = $3",
                pqxx::params{tube.order_no, tube.item_no, tube.flow_no});
            deleted_tube_rows += deleteResult.affected_rows();
        }
        spdlog::info("已删除重复流水号的管子数据，共{}根", deleted_tube_rows);

        // 在事务中同时插入管捆数据和插入管子数据

        // 3.1 插入管捆主表——————api_bundle_data_t
        // 注意：这里的SQL语句需要根据实际的表结构进行调整，确保字段名称和顺序正确
        const pqxx::result insertBundleResult = txn.exec(
            "INSERT INTO api_bundle_data_t ("
            "order_no, item_no, bundle_no, roll_no, melt_no, lot_no,"
            "prod_code, prod_cname, mat_no, mat_text, std_sg_code, std_text, sg_text,"
            "diameter, wall_thickness, weight, weight_eng, total_length, length_eng,"
            "length_from, length_to, tube, bundle_type, produce_time, ban_ci,"
            "theory_weight,theory_total_length, last_flow_no, end_type_code, end_type_sign,"
            "thread_type_code, thread_type_sign, coupling_type_code, coupling_type_sign,"
            "order_no_old, toc, gross_weight, end_type, thread_type,"
            "diameter_down_ctrl, diameter_up_ctrl, wal_thick_down_ctrl, wal_thick_up_ctrl,"
            "weight_per_meter, weight_ew, product_job_point, direction_code,pono_id_coupling,lot_no_thread) "
            "VALUES ($1,$2, $3, $4, $5, $6,"
            "$7, $8, $9, $10, $11, $12, $13,"
            "$14, $15, $16, $17, $18, $19,"
            "$20, $21, $22, $23, $24, $25,"
            "$26, $27, $28, $29, $30,"
            "$31, $32, $33, $34,"
            "$35, $36,$37, $38, $39,"
            "$40, $41, $42, $43,"
            "$44,$45,$46,$47,$48,$49)",
            pqxx::params{order_no, item_no, bundleno, roll_no, melt_no, lot_no,
                         prod_code, prod_cname, mat_no, mat_text, std_sg_code, std_text, sg_text,
                         diameter, wall_thickness, weightsum, weight_eng, lengthsum, length_eng,
                         length_from, length_to, tubecount, bundle_type_, produce_time_bundle, ban_ci_str,
                         theory_weight, theory_total_length, flow_no, end_type_code, end_type_sign,
                         thread_type_code, thread_type_sign, coupling_type_code, coupling_type_sign,
                         order_no_old, produce_time_tube, gross_weight, end_type, thread_type,
                         diameter_down_ctrl, diameter_up_ctrl, wal_thick_down_ctrl, wal_thick_up_ctrl,
                         weight_per_meter, weight_ew, product_job_point_, direction_code_, melt_no_coupling_, lot_no_coupling_});

        // 插入管子明细表
        // 循环插入每根管子数据
        for (const auto &tubePtr : Tubes())
        {
            const auto &tube = *tubePtr;
            txn.exec(
                "INSERT INTO api_tube_data_t ("
                "order_no, item_no, bundle_no, weight, length, flow_no, tube_no"
                " ) "
                "VALUES ($1, $2, $3, $4, $5, $6, $7)",
                pqxx::params{
                    tube.order_no,
                    tube.item_no,
                    bundleno,
                    std::round(tube.weight),
                    std::round(tube.length * 1000.0) / 1000.0,
                    tube.flow_no,
                    tube.tube_no});
        }
        // 更新parameter_set表中的bundle_flow_no，为下一次打捆做准备
        if (bundle_flow_no_ > 9998)
        {
            bundle_flow_no_ = 0; // 流水号达到9999后重置为0
        }
        bundle_flow_no_ += 1;
        txn.exec(
            "UPDATE parameter_set SET bundle_flow_no = $1",
            pqxx::params{bundle_flow_no_});

        // 所有操作成功，提交事务
        txn.commit();

        ApiBundleDataEvent bundleEvent;
        bundleEvent.order_no = order_no;
        bundleEvent.item_no = item_no;
        bundleEvent.bundle_no = bundleno;
        bundleEvent.flag = "I"; // 新增管捆事件
        unsigned int error;
        bool ret = writeb(m_ctx->gplatConn, "API_BUNDLE_DATA_EVENT", &bundleEvent, sizeof(bundleEvent), &error);
        spdlog::info("触发管捆事件: {}, ret={}, error={}", bundleno, ret, error);

        Clear();
    }
    catch (const std::exception &e)
    {
        spdlog::error("打捆数据库操作失败，已回滚，成品筐保留当前管子: {}", e.what());
        // pqxx::work在析构时会自动回滚，无需手动调用
        return false;
    }

    // 将下一根管捆的流水号写入Redis，NEXT_BUNDLE_FLOW_NO键用于前端显示
    m_ctx->redis->set("NEXT_BUNDLE_FLOW_NO", std::to_string(bundle_flow_no_));
    spdlog::info("下一管捆流水号已写入Redis: NEXT_BUNDLE_FLOW_NO={}", bundle_flow_no_);
    m_ctx->redis->publish("RealDataChanged", "NEXT_BUNDLE_FLOW_NO");

    // 将最近成捆的管捆号写入Redis，LATEST_BUNDLE_NO键用于前端显示
    m_ctx->redis->set("LATEST_BUNDLE_NO", bundleno);
    spdlog::info("最近成捆的管捆号已写入Redis: LATEST_BUNDLE_NO={}", bundleno);
    m_ctx->redis->publish("RealDataChanged", "LATEST_BUNDLE_NO");

    spdlog::info("=== 打捆操作成功 ===");
    spdlog::info("管捆号: {}, 根数: {}, 总重量: {}, 总长度: {}",
                 bundleno, tubecount, weightsum, lengthsum);

    // 输出插入的主表信息
    spdlog::info("已插入管捆主表: order_no={}, item_no={}, bundle_no={}",
                 order_no, item_no, bundleno);

    // 触发打标签事件
    unsigned int error;
    TagPrintEvent a;
    a.order_no = order_no;
    a.item_no = item_no;
    a.bundle_no = bundleno;
    a.count = 2;

    bool ret = writeb(m_ctx->gplatConn, "TAG_PRINT_EVENT", &a, sizeof(a), &error);
    spdlog::info("触发打标签事件: {}, ret={}, error={}", bundleno, ret, error);

    return true;
}