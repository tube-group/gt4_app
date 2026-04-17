// AlignPosition.cpp 对齐工位实现
#include "AlignPosition.h"
#include "TubeTrackContext.h"
#include "logging.h"

void CAlignPosition::UpdateForm()
{
    // 刷新对齐工位的界面显示
    const CTube *tube = Peek();
    if (m_ctx && m_ctx->redis && tube) {
        m_ctx->redis->set(REDIS_KEY, convertToJson());
        spdlog::info("AlignPosition: ALIGN_POS_TUBE_INFO updated");

        // 发布详细消息到 RealDataChanged 主题
        m_ctx->redis->publish("RealDataChanged", REDIS_KEY);
    }
}