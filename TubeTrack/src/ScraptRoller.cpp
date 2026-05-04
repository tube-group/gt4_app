// ScraptRoller.cpp 废料辊道工位实现
#include "ScraptRoller.h"
#include "TubeTrackContext.h"
#include "logging.h"

void CScraptRoller::UpdateForm()
{
    // 刷新废料辊道工位的界面显示
    if (m_ctx && m_ctx->redis) {
        m_ctx->redis->set(m_redisKey, convertToJson());
        spdlog::info("ScraptRoller: SCRAPTROLLER_POS_TUBE_INFO updated");

        // 发布详细消息到 RealDataChanged 主题
        m_ctx->redis->publish("RealDataChanged", m_redisKey);
    }
}