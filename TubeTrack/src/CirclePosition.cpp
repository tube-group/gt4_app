// CirclePosition.cpp 色环工位实现
#include "CirclePosition.h"
#include "TubeTrackContext.h"
#include "logging.h"

void CCirclePosition::UpdateForm()
{
    // 刷新色环工位的界面显示
    const CTube *tube = Peek();
    if (m_ctx && m_ctx->redis && tube) {
        m_ctx->redis->set("CIRCLE_POS_TUBE_INFO", convertToJson());
        spdlog::info("CirclePosition: CIRCLE_POS_TUBE_INFO updated");
    }
}