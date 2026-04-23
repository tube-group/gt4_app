// SprayPosition.cpp 喷印工位实现
#include "SprayPosition.h"
#include "TubeTrackContext.h"
#include "logging.h"

void CSprayPosition::UpdateForm()
{
    // 刷新喷印工位的界面显示
    if (m_ctx && m_ctx->redis) {
        m_ctx->redis->set(REDIS_KEY, convertToJson());
        spdlog::info("SprayPosition: SPRAY_POS_TUBE_INFO updated");

        // 发布详细消息到 RealDataChanged 主题
        m_ctx->redis->publish("RealDataChanged", REDIS_KEY);
    }
}