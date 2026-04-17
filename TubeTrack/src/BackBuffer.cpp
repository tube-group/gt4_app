// BackBuffer.cpp 测量点后缓冲区 实现
#include "BackBuffer.h"
#include "TubeTrackContext.h"
#include "logging.h"

void CBackBuffer::UpdateForm()
{
    // 刷新测量点后缓冲区的界面显示
    if (m_ctx && m_ctx->redis)
    {
        m_ctx->redis->set(REDIS_KEY, convertToJson());
        spdlog::info("BackBuffer: BACK_BUFFER_TUBE_INFO updated");
        // 发布详细消息到 RealDataChanged 主题
        m_ctx->redis->publish("RealDataChanged", REDIS_KEY);
    }
}

void CBackBuffer::DebugOut()
{
    spdlog::info("BackBuffer tube count: {}", Count());
}