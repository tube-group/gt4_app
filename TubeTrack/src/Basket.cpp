// Basket.cpp 成品料筐工位实现
#include "Basket.h"
#include "TubeTrackContext.h"
#include "logging.h"

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
        spdlog::info("{}: {} = {}", m_positionName, m_redisKey, convertToJson());
    }
}

void CBasket::ReadParameterSet()
{
    try
    {
        pqxx::nontransaction ntx(*m_ctx->pgConn);
        const pqxx::result result = ntx.exec(
            "SELECT bundle_number, bundle_flow_no, bundle_first_type "
            "FROM parameter_set "
            "LIMIT 1");

        if (result.empty())
        {
            spdlog::warn("parameter_set表无数据，使用默认成品料筐参数");
        }
        else
        {
            const auto &row = result[0];

            this->bundle_number_ = row["bundle_number"].as<int>();     // 打捆根数
            this->bundle_flow_no_ = row["bundle_flow_no"].as<int>();    // 管捆流水号
            this->bundle_first_type_ = row["bundle_first_type"].as<int>(); // 管捆号首位(1油管2套管）

            spdlog::info("成品料筐工位从数据库加载生产计划参数成功");
        }
    }
    catch (const std::exception &e)
    {
        spdlog::error("读取parameter_set失败，使用默认值: {}", e.what());
    }
}
