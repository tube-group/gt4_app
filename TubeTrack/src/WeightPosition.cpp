// WeightPosition.cpp 称重工位实现
#include "WeightPosition.h"
#include "TubeTrackContext.h"
#include "logging.h"

namespace {

constexpr const char *kWeightAreaCode = "AREA-A";

void PublishWeightAlarm(TubeTrackContext *ctx, const std::string &message)
{
    if (!ctx || !ctx->alarmPublisher)
    {
        return;
    }

    if (!ctx->alarmPublisher->publish(message, kWeightAreaCode))
    {
        spdlog::error("称重报警发布失败: {}", message);
    }
}

} // namespace

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
            this->weight_limit_max_ = row["weight_limit_max"].as<float>();     // 管重偏差上限
            this->weight_limit_min_ = row["weight_limit_min"].as<float>();     // 管重偏差下限
            this->weight_per_meter_ = row["weight_per_meter"].as<float>();     // 米重(kg/m)
            this->weight_ew_ = row["weight_ew"].as<float>();                   // EW值
            this->weight_coupling_ = row["weight_coupling"].as<float>();       // 保护环重量(0.01KG)

            spdlog::info("称重工位从数据库加载生产计划参数成功");
        }
    }
    catch (const std::exception &e)
    {
        spdlog::error("读取parameter_set失败，使用默认值: {}", e.what());
    }
}

void CWeightPosition::SetTubeWeight(int weight)
{
    if (m_tubes.empty())
    {
        spdlog::warn("无法设置管子重量: 工位无管子");
        return;
    }

    // 检查称重是否允许，如果称重允许
    if (weight_enable_)
    {
        // 无效重量数据处理
        if (weight < 1)
        {
            m_tubes[0]->weight = 0.0f;
            m_tubes[0]->weight_ok = false;
            m_bWbReleased = false; // 封锁步进梁，等待人工处理

            //debug: 测试环境下置管子重量为500；
            // m_tubes[0]->weight = 500.0f; // 初始化重量为500.0f
            // m_tubes[0]->weight_ok = true; // 初始化重量合格状态为true
            // m_bWbReleased = true; // 释放步进梁，允许继续生产

            // 根据不同的错误码记录日志和报警
            if (weight == -1)
            {
                spdlog::error("称重乱码，未获得有效的重量数据，请尝试人工称重");
                PublishWeightAlarm(m_ctx, "称重乱码，未获得有效的重量数据，请尝试人工称重");
            }
            else if (weight == -2)
            {
                spdlog::error("称重超时，未获得有效的重量数据，请尝试人工称重");
                PublishWeightAlarm(m_ctx, "称重超时，未获得有效的重量数据，请尝试人工称重");
            }
            else if (weight == 0)
            {
                spdlog::error("称重数据为0，未获得有效的重量数据，请尝试人工称重");
                PublishWeightAlarm(m_ctx, "称重数据为0，未获得有效的重量数据，请尝试人工称重");
            }

            m_bWbReleased = false;
        }
        else
        {
            // 读取重量kg，保护环重量0.01kg，转换为kg单位
            // 实际重量 = 读取重量 - 保护环重量
            float actualWeight = weight - weight_coupling_;
            m_tubes[0]->weight = actualWeight;
            // 没有长度数据，不进行管子判废
            spdlog::info("管子称重完成: 实际重量={}kg", actualWeight);

            m_bWbReleased = true;
        }
    }
    else
    {
        // 称重未使能：跳过称重逻辑
        m_tubes[0]->weight = 0.0f;
        m_tubes[0]->weight_ok = false;
        m_bWbReleased = true;

        spdlog::info("称重功能未使能，跳过称重");
    }

    UpdateForm(); // 更新称重工位显示
}

void CWeightPosition::EntryTriggerBeforePush(CTube &tube)
{
}

void CWeightPosition::EntryTrigger(const CTube &tube)
{
}
