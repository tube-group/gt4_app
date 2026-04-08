// ProductionPlan.cpp 投料计划实现
#include "ProductionPlan.h"
#include "TubeTrackContext.h"
#include <nlohmann/json.hpp>

// bool CProductionPlan::Pop(CTube *pTube, int /*mode*/)
// {
//     static int flow_no = 0;
//     /* 填充管子数据 */;
//     (*pTube).calib_tube = false;
//     (*pTube).order_no = "12345678";
//     (*pTube).item_no = "ITEM001";
//     (*pTube).roll_no = "ROLL1234";
//     (*pTube).melt_no = "MELT5678";
//     (*pTube).lot_no = "LOT91011";
//     (*pTube).tube_no = 1;
//     (*pTube).flow_no = flow_no++;
//     (*pTube).lotno_coupling = "COUPLELOT";
//     (*pTube).meltno_coupling = "COUPLEMELT";
//     (*pTube).length = 6.0f;
//     (*pTube).weight = 12.5f;
//     (*pTube).lengthOk = true;
//     (*pTube).weightOk = true;
//     (*pTube).bSprayed = false;

//     // 投料计划的出栈逻辑实现
//     return true; // 示例返回值
// }

bool CProductionPlan::Pop(CTube *pTube, int /*mode*/)
{

    if (feed_num > 0)
    {
        // 查询当前合同和生产参数，填充管子数据
        pTube->order_no = order_no;
        pTube->roll_no = roll_no;
        pTube->item_no = item_no;
        pTube->lot_no = lot_no;
        pTube->melt_no = melt_no;
        pTube->meltno_coupling = meltno_coupling;
        pTube->lotno_coupling = lotno_coupling;
        pTube->tube_no = tube_no;

        // 生成流水号
        static int flow_no = 0;
        pTube->flow_no = flow_no++;

        // 更新计数器
        feed_num--;
        tube_no += 1;

        UpdateForm();

        return true;
    }
    else
    {
        return false;
    }
}

string CProductionPlan::convertToJson(const CProductionPlan &plan)
{
    // 使用nlohmann/json库实现生产计划转换为JSON格式字符串
    nlohmann::json j;
    j["order_no"] = plan.order_no;
    j["item_no"] = plan.item_no;
    j["roll_no"] = plan.roll_no;
    j["melt_no"] = plan.melt_no;
    j["lot_no"] = plan.lot_no;
    j["lotno_coupling"] = plan.lotno_coupling;
    j["meltno_coupling"] = plan.meltno_coupling;
    j["feed_num"] = plan.feed_num;
    j["tube_no"] = plan.tube_no;

    return j.dump(4);
}

// 初始化静态常量
// const char *CProductionPlan::REDIS_KEY = "PlanInfo";

void CProductionPlan::UpdateForm()
{
    // 检查上下文和Redis连接
    if (m_ctx == nullptr || m_ctx->redis == nullptr)
    {
        std::cerr << "错误：上下文或Redis连接未初始化" << std::endl;
        return;
    }

    auto* redis = m_ctx->redis.get();

    try
    {
        // 转化为json格式输出
        string jsonStr = convertToJson(*this);

        // 检查JSON是否有效
        if (jsonStr.empty())
        {
            std::cerr << "错误：生成的JSON字符串为空！" << std::endl;
            return;
        }

        std::cout << "生成的JSON: " << jsonStr << std::endl;

        // 写入Redis数据库
        redis->set(REDIS_KEY, jsonStr);

        // 发布详细消息到 RealDataChanged 主题
        redis->publish("RealDataChanged", REDIS_KEY);

        std::cout << "✓ 已更新Redis并发布通知:" << REDIS_KEY << std::endl;
    }
    catch (const std::exception &e)
    {
        std::cerr << "UpdateForm异常: " << e.what() << std::endl;
    }
}

void CProductionPlan::Initialize()
{
    // 初始化生产计划数据，从数据库表parameter_set中读取参数
    // 检查上下文和PostgreSQL连接
    if (m_ctx == nullptr || m_ctx->pgConn == nullptr)
    {
        std::cerr << "错误：上下文或PostgreSQL连接未初始化" << std::endl;
        return;
    }
    // 连接pg数据库，查询parameter_set表中的参数
    auto *pgConn = m_ctx->pgConn.get();

    try
    {
        pqxx::work txn(*pgConn);
        const pqxx::result result = txn.exec(
            "SELECT order_no, item_no, roll_no, melt_no, lot_no, "
            "lot_no_coupling, melt_no_coupling, feed_number, tube_no "
            "FROM parameter_set "
            "LIMIT 1");
        txn.commit();

        if (result.empty())
        {
            std::cerr << "警告：parameter_set表无数据" << std::endl;
        }
        else
        {
            const auto &row = result[0];

            this->order_no = row["order_no"].as<string>(this->order_no);                       // 合同号
            this->item_no = row["item_no"].as<string>(this->item_no);                          // 项目号
            this->roll_no = row["roll_no"].as<string>(this->roll_no);                          // 轧批号
            this->melt_no = row["melt_no"].as<string>(this->melt_no);                          // 炉号
            this->lot_no = row["lot_no"].as<string>(this->lot_no);                             // 试批号
            this->lotno_coupling = row["lot_no_coupling"].as<string>(this->lotno_coupling);    // 接箍批号
            this->meltno_coupling = row["melt_no_coupling"].as<string>(this->meltno_coupling); // 接箍炉号
            this->feed_num = row["feed_number"].as<int>(this->feed_num);                       // 投料支数
            this->tube_no = row["tube_no"].as<int>(this->tube_no);                             // 管号
        
            std::cout << "从数据库加载生产计划参数成功:" << std::endl;
        }

    }
    catch (const std::exception &e)
    {
        std::cerr << "读取parameter_set失败，使用默认值: " << e.what() << std::endl;
    }

    // 初始同步到Redis
    UpdateForm();
}
