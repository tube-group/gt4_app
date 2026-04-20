#include "workthread.h"

#include <algorithm>
#include <chrono>
#include <cstdint>
#include <iomanip>
#include <sstream>
#include <thread>

namespace
{
    // 订阅所有点位和定时器，失败时记录日志但不抛异常，由调用者决定重连逻辑
    bool subscribeAllTags(const MonitorPlcDataContext &ctx)
    {
        unsigned int error = 0; // 用于接收错误码

        if (!subscribe(ctx.gplatConn, "timer_500ms", &error))
        {
            spdlog::error("订阅 timer_500ms 失败，错误码: {}", error);
            return false;
        }

        for (const auto &tag : ctx.tags)
        {
            if (!subscribe(ctx.gplatConn, tag.name.c_str(), &error))
            {
                spdlog::error("订阅点位失败: {}, 错误码: {}", tag.name, error);
                return false;
            }
        }

        spdlog::info("完成 PLC 点位订阅，共 {} 个", ctx.tags.size());
        return true;
    }
    // 格式化 REAL 类型为字符串，保留6位小数
    std::string formatReal(float value)
    {
        std::ostringstream stream;
        stream << std::setprecision(6) << value;
        return stream.str();
    }
    // 根据点位定义格式化值为字符串，便于存储到Redis
    std::string formatValue(const TagDefinition &tag, const char *buffer)
    {
        if (tag.type == "BOOL")
        {
            return read_value<bool>(buffer) ? "true" : "false";
        }
        if (tag.type == "INT")
        {
            return std::to_string(read_value<std::int16_t>(buffer));
        }
        if (tag.type == "WORD")
        {
            return std::to_string(read_value<std::uint16_t>(buffer));
        }
        if (tag.type == "DINT")
        {
            return std::to_string(read_value<std::int32_t>(buffer));
        }
        if (tag.type == "DWORD")
        {
            return std::to_string(read_value<std::uint32_t>(buffer));
        }
        if (tag.type == "REAL")
        {
            return formatReal(read_value<float>(buffer));
        }
        if (tag.type == "STRING")
        {
            return read_value<std::string>(buffer);
        }

        throw std::runtime_error("不支持的数据类型: " + tag.type + ", tag=" + tag.name);
    }
    // 连接失败或订阅失败时的重连逻辑
    bool reconnectAndResubscribe(MonitorPlcDataContext &ctx, const AppConfig &app, volatile sig_atomic_t &running)
    {
        ctx.Cleanup();
        while (running)
        {
            if (initGplat(ctx) && subscribeAllTags(ctx))
            {
                return true;
            }

            ctx.Cleanup();
            std::this_thread::sleep_for(std::chrono::milliseconds(app.reconnectIntervalMs));
        }
        return false;
    }

} // namespace

void workThread(MonitorPlcDataContext &ctx, const AppConfig &app, volatile sig_atomic_t &running)
{
    // 订阅失败，可能是连接问题，尝试重连
    if (!subscribeAllTags(ctx))
    {
        if (!reconnectAndResubscribe(ctx, app, running))
        {
            return;
        }
    }

    unsigned int error = 0; // 用于接收错误码
    char value[4096] = {0}; // 接收缓冲区
    std::string tagName;    // 接收点位名称

    while (running)
    {
        tagName.clear(); // 清空点位名称
        value[0] = '\0';

        const bool ret = waitpostdata(ctx.gplatConn, tagName, value, static_cast<int>(sizeof(value)), -1, &error);
        if (!ret)
        {
            spdlog::warn("waitpostdata 失败，准备重连，错误码: {}", error);
            if (!reconnectAndResubscribe(ctx, app, running))
            {
                return;
            }
            continue;
        }

        if (tagName == "timer_500ms")
        {
            continue;
        }

        const auto it = ctx.tagMap.find(tagName); // 查找点位定义
        // 未找到点位定义，记录日志并继续循环
        if (it == ctx.tagMap.end())
        {
            spdlog::warn("收到未注册点位变化: {}", tagName);
            continue;
        }
        // 处理点位变化，格式化值并写入Redis
        try
        {
            // 根据点位定义格式化值为字符串，便于存储到Redis
            const std::string redisValue = formatValue(it->second, value);

            // 关键修改：WATCHDOG信号只记录不发布
            if (tagName == "WATCHDOG" || tagName == "WATCHDOG2")
            {
                // 只记录debug日志，不操作Redis
                spdlog::debug("WATCHDOG心跳: {}={}", tagName, redisValue);
                continue; // 跳过Redis操作
            }

            // 其他正常点位写入Redis
            ctx.redis->set(tagName, redisValue); // 将变化写入Redis
            ctx.redis->publish(app.redisChannel, tagName); // 发布消息通知订阅者点位变化
            spdlog::info("PLC点位变化已写入Redis: {}={}", tagName, redisValue);
        }
        catch (const std::exception &ex)
        {
            spdlog::error("处理点位 {} 失败: {}", tagName, ex.what());
        }
    }
}