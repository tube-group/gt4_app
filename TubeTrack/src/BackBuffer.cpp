// BackBuffer.cpp 测量点后缓冲区 实现
#include "BackBuffer.h"
#include "TubeTrackContext.h"
#include "logging.h"

void CBackBuffer::UpdateForm()
{
    // 刷新测量点后缓冲区的界面显示
    if (m_ctx && m_ctx->redis)
    {
        m_ctx->redis->set(m_redisKey, convertToJson());
        spdlog::info("{}: {} updated", m_positionName, m_redisKey);
        // 发布详细消息到 RealDataChanged 主题
        m_ctx->redis->publish("RealDataChanged", m_redisKey);
    }
}

void CBackBuffer::DebugOut()
{
    spdlog::info("{} tube count: {}", m_positionName, Count());
    // 输出测量点后缓冲区管子数量
    if (m_ctx && m_ctx->redis)
    {
        // spdlog::info("{}: {} = {}", m_positionName, m_redisKey, convertToJson());
    }
}

bool CBackBuffer::QueryTube(const std::string& order_no, int flow_no) const
{
    // 根据订单号和流水号查询管子是否在后缓冲区中
    for(const auto& tube : Tubes())
    {
        if (tube->order_no == order_no && tube->flow_no == flow_no)
        {
            return true; // 找到匹配的管子，返回true
        }
    }
    return false; // 没有找到匹配的管子，返回false
}