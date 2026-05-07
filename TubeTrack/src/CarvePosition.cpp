// CarvePosition.cpp 刻印工位实现
#include "CarvePosition.h"
#include "TubeTrackContext.h"
#include "logging.h"

void CCarvePosition::UpdateForm()
{
    // 刷新刻印工位的界面显示
    if (m_ctx && m_ctx->redis) {
        m_ctx->redis->set(m_redisKey, convertToJson());
        spdlog::info("{}: {} updated", m_positionName, m_redisKey);

        // 发布详细消息到 RealDataChanged 主题
        m_ctx->redis->publish("RealDataChanged", m_redisKey);
    }
}
void CCarvePosition::ReadParameterSet()
{
    try
    {
        pqxx::nontransaction ntx(*m_ctx->pgConn);
        const pqxx::result result = ntx.exec(
            "SELECT carve_enable "
            "FROM parameter_set "
            "LIMIT 1");
        if (result.empty())
        {
            spdlog::warn("parameter_set表无数据，使用默认刻印工位参数");
        }
        else
        {
            const auto &row = result[0];
            this->carve_enable_ = row["carve_enable"].as<int>(); // 刻印允许

            spdlog::info("刻印工位从数据库加载生产计划参数成功");
        }
    }
    catch (const std::exception &e)
    {
        spdlog::error("读取parameter_set失败，使用默认值: {}", e.what());
    }
}