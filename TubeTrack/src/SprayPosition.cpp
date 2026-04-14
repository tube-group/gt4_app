// SprayPosition.cpp 喷印工位实现
#include "SprayPosition.h"
#include "TubeTrackContext.h"
#include "logging.h"

void CSprayPosition::UpdateForm()
{
    // 刷新喷印工位的界面显示
    const CTube *tube = Peek();
    if (m_ctx && m_ctx->redis && tube) {
        m_ctx->redis->set("SPRAY_POS_TUBE_INFO", convertToJson());
        spdlog::info("SprayPosition: SPRAY_POS_TUBE_INFO updated");
    }
}