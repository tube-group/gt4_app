// CirclePosition.cpp 色环工位实现
#include "CirclePosition.h"
#include "TubeTrackContext.h"
#include "logging.h"

void CCirclePosition::UpdateForm()
{
    // 刷新色环工位的界面显示
    if (m_ctx && m_ctx->redis)
    {
        m_ctx->redis->set(m_redisKey, convertToJson());
        spdlog::info("{}: {} updated", m_positionName, m_redisKey);

        // 发布详细消息到 RealDataChanged 主题
        m_ctx->redis->publish("RealDataChanged", m_redisKey);
    }
}

void CCirclePosition::EntryTrigger(const CTube &tube)
{
    // 当管子进入色环工位时触发
    unsigned int err;
    bool status = tube.length_ok && tube.weight_ok;
    write_plc_bool(m_ctx->gplatConn, "CIRCLE_ENABLE", status, &err);
    spdlog::info("通知PLC色环工位的管子是否为废管: status={}", status);
}

void CCirclePosition::ReadParameterSet()
{
    try
    {
        pqxx::nontransaction ntx(*m_ctx->pgConn);
        const pqxx::result result = ntx.exec(
            "SELECT circle_enable,gun1,gun2,gun3,gun4,gun5,diameter "
            "FROM parameter_set "
            "LIMIT 1");
        if (result.empty())
        {
            spdlog::warn("parameter_set表无数据，使用默认色环工位参数");
        }
        else
        {
            const auto &row = result[0];

            this->circle_enable_ = row["circle_enable"].as<int>(); // 色环允许

            spdlog::info("色环工位从数据库加载生产计划参数成功");

            unsigned int err;
            write_plc_bool(m_ctx->gplatConn, "CIRCLE_ENABLE", circle_enable_ != 0, &err);
            write_plc_bool(m_ctx->gplatConn, "CIRCLE1_ENABLE", row["gun1"].as<int>() != 0, &err);
            write_plc_bool(m_ctx->gplatConn, "CIRCLE2_ENABLE", row["gun2"].as<int>() != 0, &err);
            write_plc_bool(m_ctx->gplatConn, "CIRCLE3_ENABLE", row["gun3"].as<int>() != 0, &err);
            write_plc_bool(m_ctx->gplatConn, "CIRCLE4_ENABLE", row["gun4"].as<int>() != 0, &err);
            write_plc_bool(m_ctx->gplatConn, "CIRCLE5_ENABLE", row["gun5"].as<int>() != 0, &err);
            write_plc_float(m_ctx->gplatConn, "TUBE_DIA", row["diameter"].as<float>(), &err);
        }
    }
    catch (const std::exception &e)
    {
        spdlog::error("读取parameter_set失败，使用默认值: {}", e.what());
    }
}