// ProductionPlan.cpp 投料计划实现
#include "ProductionPlan.h"
#include "TubeTrackContext.h"
#include <memory>
#include <nlohmann/json.hpp>
#include "../../include/logging.h" // spdlog

std::unique_ptr<CTube> CProductionPlan::Pop(int /*mode*/)
{

    if (feed_num > 0)
    {
        auto tube = std::make_unique<CTube>();
        // 查询当前合同和生产参数，填充管子数据
        tube->order_no = order_no;
        tube->roll_no = roll_no;
        tube->item_no = item_no;
        tube->lot_no = lot_no;
        tube->melt_no = melt_no;
        tube->meltno_coupling = meltno_coupling;
        tube->lotno_coupling = lotno_coupling;
        tube->tube_no = tube_no;

        // 生成流水号
        static int flow_no = 0;
        tube->flow_no = flow_no++;

        // 更新计数器
        feed_num--;
        tube_no += 1;

        UpdateForm();

        return tube;
    }
    else
    {
        return nullptr;
    }
}

string CProductionPlan::convertToJson()
{
    // 使用nlohmann/json库实现生产计划转换为JSON格式字符串
    nlohmann::json j;
    j["order_no"] = order_no;
    j["item_no"] = item_no;
    j["roll_no"] = roll_no;
    j["melt_no"] = melt_no;
    j["lot_no"] = lot_no;
    j["lotno_coupling"] = lotno_coupling;
    j["meltno_coupling"] = meltno_coupling;
    j["feed_num"] = feed_num;
    j["tube_no"] = tube_no;

    return j.dump(4);
}

void CProductionPlan::UpdateForm()
{
    try
    {
        // 转化为json格式输出
        string jsonStr = convertToJson();

        // 检查JSON是否有效
        if (jsonStr.empty())
        {
            spdlog::error("错误：生成的JSON字符串为空！");
            return;
        }

        spdlog::info("生成的JSON: {}", jsonStr);

        // 写入Redis数据库
        m_ctx->redis->set(REDIS_KEY, jsonStr);

        // 发布详细消息到 RealDataChanged 主题
        m_ctx->redis->publish("RealDataChanged", REDIS_KEY);

        spdlog::info("生产计划已更新并发布到Redis: {}", jsonStr);
    }
    catch (const std::exception &e)
    {
        spdlog::error("UpdateForm异常: {}", e.what());
    }
}

void CProductionPlan::Initialize()
{
    try
    {
        pqxx::nontransaction ntx(*m_ctx->pgConn);
        const pqxx::result result = ntx.exec(
            "SELECT order_no, item_no, roll_no, melt_no, lot_no, "
            "lot_no_coupling, melt_no_coupling, feed_number, tube_no "
            "FROM parameter_set "
            "LIMIT 1");

        if (result.empty())
        {
            spdlog::warn("parameter_set表无数据，使用默认生产计划参数");
        }
        else
        {
            const auto &row = result[0];

            this->order_no = row["order_no"].as<string>();                       // 合同号
            this->item_no = row["item_no"].as<string>();                          // 项目号
            this->roll_no = row["roll_no"].as<string>();                          // 轧批号
            this->melt_no = row["melt_no"].as<string>();                          // 炉号
            this->lot_no = row["lot_no"].as<string>();                             // 试批号
            this->lotno_coupling = row["lot_no_coupling"].as<string>();    // 接箍批号
            this->meltno_coupling = row["melt_no_coupling"].as<string>(); // 接箍炉号
            this->feed_num = row["feed_number"].as<int>();                       // 投料支数
            this->tube_no = row["tube_no"].as<int>();                             // 管号
        
            spdlog::info("从数据库加载生产计划参数成功  order_no: {}", this->order_no);
        }

    }
    catch (const std::exception &e)
    {
        spdlog::error("读取parameter_set失败，使用默认值: {}", e.what());
    }

    // 初始同步到Redis
    UpdateForm();
}
