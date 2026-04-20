// Basket.cpp 成品料筐工位实现
#include "Basket.h"
#include "TubeTrackContext.h"
#include "logging.h"

void CBasket::UpdateForm()
{
    // 刷新成品料筐的界面显示
    if (m_ctx && m_ctx->redis)
    {
        m_ctx->redis->set(REDIS_KEY, convertToJson());
        spdlog::info("Basket: BASKET_TUBE_INFO updated");
        // 发布详细消息到 RealDataChanged 主题
        m_ctx->redis->publish("RealDataChanged", REDIS_KEY);
    }
}

void CBasket::DebugOut()
{
    // 输出成品料筐管子数量
    if (m_ctx && m_ctx->redis)
    {
        spdlog::info("Basket: BASKET_TUBE_INFO = {}", convertToJson());
    }
}
