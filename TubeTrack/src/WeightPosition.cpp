// WeightPosition.cpp 称重工位实现
#include "WeightPosition.h"
#include "TubeTrackContext.h"
#include "logging.h"

void CWeightPosition::UpdateForm()
{
    // 刷新称重工位的界面显示
    if (m_ctx && m_ctx->redis)
    {
        m_ctx->redis->set(m_redisKey, convertToJson());
        spdlog::info("{}: {} updated", m_positionName, m_redisKey);

        // 发布详细消息到 RealDataChanged 主题
        m_ctx->redis->publish("RealDataChanged", m_redisKey);
    }
}

void CWeightPosition::ReadParameterSet()
{
    try
    {
        pqxx::nontransaction ntx(*m_ctx->pgConn);
        const pqxx::result result = ntx.exec(
            "SELECT weight_enable, waste_weight_enable, weight_limit_max, weight_limit_min, "
            "weight_per_meter, weight_ew, weight_coupling "
            "FROM parameter_set "
            "LIMIT 1");
        if (result.empty())
        {
            spdlog::warn("parameter_set表无数据，使用默认称重工位参数");
        }
        else
        {
            const auto &row = result[0];

            this->weight_enable_ = row["weight_enable"].as<int>();             // 称重允许
            this->waste_weight_enable_ = row["waste_weight_enable"].as<int>(); // 重量判废
            this->weight_limit_max_ = row["weight_limit_max"].as<float>();       // 管重偏差上限
            this->weight_limit_min_ = row["weight_limit_min"].as<float>();       // 管重偏差下限
            this->weight_per_meter_ = row["weight_per_meter"].as<float>();       // 米重(kg/m)
            this->weight_ew_ = row["weight_ew"].as<float>();                     // EW值
            this->weight_coupling_ = row["weight_coupling"].as<int>();         // 保护环重量(0.01KG)

            spdlog::info("称重工位从数据库加载生产计划参数成功");
        }
    }
    catch (const std::exception &e)
    {
        spdlog::error("读取parameter_set失败，使用默认值: {}", e.what());
    }
}