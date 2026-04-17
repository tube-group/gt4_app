// CirclePosition.cpp 色环工位实现
#include "CirclePosition.h"
#include "TubeTrackContext.h"
#include "logging.h"

void CCirclePosition::UpdateForm()
{
    // 刷新色环工位的界面显示
    const CTube *tube = Peek();
    if (m_ctx && m_ctx->redis && tube)
    {
        m_ctx->redis->set(REDIS_KEY, convertToJson());
        spdlog::info("CirclePosition: CIRCLE_POS_TUBE_INFO updated");

        // 发布详细消息到 RealDataChanged 主题
        m_ctx->redis->publish("RealDataChanged", REDIS_KEY);
    }
}