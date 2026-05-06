// AlignPosition.cpp 对齐工位实现
#include "AlignPosition.h"
#include "TubeTrackContext.h"
#include "logging.h"

void CAlignPosition::UpdateForm()
{
    // 刷新对齐工位的界面显示
    if (m_ctx && m_ctx->redis) {
        m_ctx->redis->set(m_redisKey, convertToJson());
        spdlog::info("{}: {} updated", m_positionName, m_redisKey);

        // 发布详细消息到 RealDataChanged 主题
        m_ctx->redis->publish("RealDataChanged", m_redisKey);
    }
}