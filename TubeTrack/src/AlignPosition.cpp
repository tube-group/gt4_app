// AlignPosition.cpp 对齐工位实现
#include "AlignPosition.h"
#include "TubeTrackContext.h"
#include "logging.h"

void CAlignPosition::UpdateForm()
{
    // 刷新对齐工位的界面显示
    const CTube *tube = Peek();
    if (m_ctx && m_ctx->redis && tube) {
        m_ctx->redis->set("ALIGN_POS_TUBE_INFO", convertToJson());
        spdlog::info("AlignPosition: ALIGN_POS_TUBE_INFO updated");
    }
}