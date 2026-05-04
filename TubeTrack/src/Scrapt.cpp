// Scrapt.cpp 废料筐工位实现
#include "Scrapt.h"
#include "TubeTrackContext.h"
#include "logging.h"


void CScrapt::UpdateForm()
{
    // 刷新废料筐工位的界面显示
    if (m_ctx && m_ctx->redis) {
        m_ctx->redis->set(m_redisKey, convertToJson());
        spdlog::info("Scrapt: SCRAPT_POS_TUBE_INFO updated");

        // 发布详细消息到 RealDataChanged 主题
        m_ctx->redis->publish("RealDataChanged", m_redisKey);
    }
}

void CScrapt::DebugOut()
{
    // 输出废料筐管子数量
    if (m_ctx && m_ctx->redis)
    {
        spdlog::info("Scrapt: SCRAPT_POS_TUBE_INFO = {}", convertToJson());
    }
}