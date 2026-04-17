// CarvePosition.cpp 刻印工位实现
#include "CarvePosition.h"
#include "TubeTrackContext.h"
#include "logging.h"

void CCarvePosition::UpdateForm()
{
    // 刷新刻印工位的界面显示
    const CTube *tube = Peek();
    if (m_ctx && m_ctx->redis && tube) {
        m_ctx->redis->set(REDIS_KEY, convertToJson());
        spdlog::info("CarvePosition: CARVE_POS_TUBE_INFO updated");

        // 发布详细消息到 RealDataChanged 主题
        m_ctx->redis->publish("RealDataChanged", REDIS_KEY);
    }
}