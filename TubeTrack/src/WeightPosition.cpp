// WeightPosition.cpp 称重工位实现
#include "WeightPosition.h"
#include "TubeTrackContext.h"
#include "logging.h"

#include <ctime>
#include <iomanip>
#include <nlohmann/json.hpp>
#include <sstream>

namespace {

constexpr const char *kWeightAlarmCode = "WEIGHT_DATA_INVALID";
constexpr const char *kWeightAreaCode = "AREA-A";
constexpr const char *kWeightAlarmSeverity = "major";
constexpr const char *kWeightAlarmSourceModule = "TubeTrack";
constexpr const char *kWeightAlarmSourceKey = "weight.position.measurement";
constexpr const char *kWeightAlarmDedupeKey = "tubetrack:weight-position:invalid-data";

std::string BuildOccurredAtUtc()
{
    const auto now = std::time(nullptr);
    std::tm utcTime{};
    gmtime_r(&now, &utcTime);

    std::ostringstream stream;
    stream << std::put_time(&utcTime, "%Y-%m-%dT%H:%M:%SZ");
    return stream.str();
}

std::string BuildWeightAlarmDetailJson(const CTube &tube, int rawWeight, bool weightEnabled)
{
    nlohmann::json detailJson;
    detailJson["rawWeight"] = rawWeight;
    detailJson["weightEnable"] = weightEnabled;
    detailJson["tubeNo"] = tube.tube_no;
    detailJson["flowNo"] = tube.flow_no;
    detailJson["orderNo"] = tube.order_no;
    detailJson["itemNo"] = tube.item_no;
    return detailJson.dump();
}

void RaiseWeightAlarm(TubeTrackContext *ctx, const CTube &tube, int rawWeight, const std::string &message)
{
    if (!ctx || !ctx->alarmPublisher)
    {
        return;
    }

    AlarmRaiseRequest request;
    request.alarmCode = kWeightAlarmCode;
    request.areaCode = kWeightAreaCode;
    request.severity = kWeightAlarmSeverity;
    request.sourceModule = kWeightAlarmSourceModule;
    request.sourceKey = kWeightAlarmSourceKey;
    request.title = "称重工位无效重量数据";
    request.message = message;
    request.detailJson = BuildWeightAlarmDetailJson(tube, rawWeight, true);
    request.requireAck = true;
    request.autoClear = false;
    request.dedupeKey = kWeightAlarmDedupeKey;
    request.occurredAt = BuildOccurredAtUtc();

    const AlarmPublishResult result = ctx->alarmPublisher->Raise(request);
    if (!result.ok)
    {
        spdlog::error("称重报警发布失败: {}", result.errorMessage);
    }
}

void ClearWeightAlarm(TubeTrackContext *ctx, const CTube &tube, int rawWeight, bool weightEnabled)
{
    if (!ctx || !ctx->alarmPublisher)
    {
        return;
    }

    AlarmClearRequest request;
    request.alarmCode = kWeightAlarmCode;
    request.areaCode = kWeightAreaCode;
    request.sourceModule = kWeightAlarmSourceModule;
    request.sourceKey = kWeightAlarmSourceKey;
    request.dedupeKey = kWeightAlarmDedupeKey;
    request.occurredAt = BuildOccurredAtUtc();
    request.severity = kWeightAlarmSeverity;
    request.title = "称重工位无效重量数据";
    request.message = weightEnabled ? "称重数据恢复正常" : "称重功能未使能，报警已清除";
    request.detailJson = BuildWeightAlarmDetailJson(tube, rawWeight, weightEnabled);

    const AlarmPublishResult result = ctx->alarmPublisher->Clear(request);
    if (!result.ok)
    {
        spdlog::error("称重报警清除失败: {}", result.errorMessage);
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
    // 获取当前工位的管子对象
    CTube *tube = const_cast<CTube *>(Peek());
    if (!tube)
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
            tube->weight = 0.0f;
            tube->weight_ok = false;
            m_bWbReleased = false; // 封锁步进梁，等待人工处理

            // 根据不同的错误码记录日志和报警
            if (weight == -1)
            {
                spdlog::error("称重乱码，未获得有效的重量数据，请尝试人工称重");
                RaiseWeightAlarm(m_ctx, *tube, weight, "称重乱码，未获得有效的重量数据，请尝试人工称重");
            }
            else if (weight == -2)
            {
                spdlog::error("称重超时，未获得有效的重量数据，请尝试人工称重");
                RaiseWeightAlarm(m_ctx, *tube, weight, "称重超时，未获得有效的重量数据，请尝试人工称重");
            }
            else if (weight == 0)
            {
                spdlog::error("称重数据为0，未获得有效的重量数据，请尝试人工称重");
                RaiseWeightAlarm(m_ctx, *tube, weight, "称重数据为0，未获得有效的重量数据，请尝试人工称重");
            }
        }
        else
        {
            // 读取重量kg，保护环重量0.01kg，转换为kg单位
            // 实际重量 = 读取重量 - 保护环重量
            float actualWeight = weight - weight_coupling_;
            tube->weight = actualWeight;
            // 没有长度数据，不进行管子判废
            spdlog::info("管子称重完成: 实际重量={}kg", actualWeight);
            ClearWeightAlarm(m_ctx, *tube, weight, true);
            UpdateForm(); // 更新称重工位显示
        }
    }
    else
    {
        // 称重未使能：跳过称重逻辑
        tube->weight = 0.0f;
        tube->weight_ok = false;
        m_bWbReleased = true;

        spdlog::info("称重功能未使能，跳过称重");
        ClearWeightAlarm(m_ctx, *tube, weight, false);
    }
}

void CWeightPosition::EntryTrigger(const CTube &tube)
{
    // Push管子进入称重工位后触发称重功能
    unsigned int error;
    int a = 1;
    bool ret = writeb(m_ctx->gplatConn, "START_WEIGHT_EVENT", &a, sizeof(a), &error);
    spdlog::info("EntryTrigger: START_WEIGHT_EVENT triggered for tube {}, writeb return: {}, error code: {}", tube.tube_no, ret, error);

    if (!ret)
    {
        spdlog::error("writeb failed with return code: {}", ret);
    }
}
