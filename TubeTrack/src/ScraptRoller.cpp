// ScraptRoller.cpp 废料辊道工位实现
#include "ScraptRoller.h"
#include "TubeTrackContext.h"
#include "logging.h"

void CScraptRoller::UpdateForm()
{
    // 刷新废料辊道工位的界面显示
    if (m_ctx && m_ctx->redis) {
        m_ctx->redis->set(m_redisKey, convertToJson());
        spdlog::info("{}: {} updated", m_positionName, m_redisKey);

        // 发布详细消息到 RealDataChanged 主题
        m_ctx->redis->publish("RealDataChanged", m_redisKey);
    }
}

void CScraptRoller::EntryTrigger(const CTube &tube)
{
    // 当管子进入废料辊道工位时触发
    unsigned int err;
    ushort status = (tube.length_ok && tube.weight_ok) ? (ushort)1 : (ushort)3;
    write_plc_ushort(m_ctx->gplatConn, "WASTE_ROLLER_FLAG", status, &err);
    spdlog::info("通知PLC出废工位的管子是否为废管: status={}", status);
}