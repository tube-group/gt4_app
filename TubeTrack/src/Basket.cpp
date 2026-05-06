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
