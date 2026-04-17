// WeightPosition.cpp 称重工位实现
#include "WeightPosition.h"
#include "TubeTrackContext.h"
#include "logging.h"

void CWeightPosition::UpdateForm()
{
    // 刷新称重工位的界面显示
    const CTube *tube = Peek();
    if (m_ctx && m_ctx->redis && tube)
    {
        m_ctx->redis->set(REDIS_KEY, convertToJson());
    }
    spdlog::info("WeightPosition: WEIGHT_POS_TUBE_INFO updated");

    // 发布详细消息到 RealDataChanged 主题
    m_ctx->redis->publish("RealDataChanged", REDIS_KEY);
}