#include "workthread.h"

#include <algorithm>
#include <array>
#include <chrono>
#include <cstdint>
#include <iomanip>
#include <stdexcept>
#include <sstream>
#include <thread>

namespace
{
    enum class PlcTagType
    {
        Bool,
        Int,
        Word,
        DInt,
        DWord,
        Real,
        String,
    };

    PlcTagType resolveTagType(const TagDefinition &tag)
    {
        if (tag.type == "BOOL")
        {
            return PlcTagType::Bool;
        }
        if (tag.type == "INT")
        {
            return PlcTagType::Int;
        }
        if (tag.type == "WORD")
        {
            return PlcTagType::Word;
        }
        if (tag.type == "DINT")
        {
            return PlcTagType::DInt;
        }
        if (tag.type == "DWORD")
        {
            return PlcTagType::DWord;
        }
        if (tag.type == "REAL")
        {
            return PlcTagType::Real;
        }
        if (tag.type == "STRING")
        {
            return PlcTagType::String;
        }

        throw std::runtime_error("不支持的数据类型: " + tag.type + ", tag=" + tag.name);
    }

    // --------订阅所有点位和定时器---------
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
    // --------根据点位定义格式化值为字符串，便于存储到Redis---------
    std::string formatValue(const TagDefinition &tag, const char *buffer)
    {
        switch (resolveTagType(tag))
        {
        case PlcTagType::Bool:
            return read_value<bool>(buffer) ? "true" : "false";
        case PlcTagType::Int:
            return std::to_string(read_value<std::int16_t>(buffer));
        case PlcTagType::Word:
            return std::to_string(read_value<std::uint16_t>(buffer));
        case PlcTagType::DInt:
            return std::to_string(read_value<std::int32_t>(buffer));
        case PlcTagType::DWord:
            return std::to_string(read_value<std::uint32_t>(buffer));
        case PlcTagType::Real:
            return formatReal(read_value<float>(buffer));
        case PlcTagType::String:
            return read_value<std::string>(buffer);
        }

        throw std::runtime_error("未处理的点位类型: " + tag.type + ", tag=" + tag.name);
    }
    // --------根据点位类型获取二进制数据大小，用于读取和格式化---------
    int getBinaryTagSize(const TagDefinition &tag)
    {
        switch (resolveTagType(tag))
        {
        case PlcTagType::Bool:
            return static_cast<int>(sizeof(bool));
        case PlcTagType::Int:
            return static_cast<int>(sizeof(std::int16_t));
        case PlcTagType::Word:
            return static_cast<int>(sizeof(std::uint16_t));
        case PlcTagType::DInt:
            return static_cast<int>(sizeof(std::int32_t));
        case PlcTagType::DWord:
            return static_cast<int>(sizeof(std::uint32_t));
        case PlcTagType::Real:
            return static_cast<int>(sizeof(float));
        case PlcTagType::String:
            throw std::logic_error("STRING类型没有固定大小，不应调用此函数");
        }

        return 0;
    }

    // --------读取当前点位值的字符串表示，供启动时同步使用---------
    std::string readCurrentValue(const MonitorPlcDataContext &ctx, const TagDefinition &tag, unsigned int &error)
    {
        if (resolveTagType(tag) == PlcTagType::String)
        {
            char rawValue[4096] = {0};
            if (!readb_string(ctx.gplatConn, tag.name.c_str(), rawValue, static_cast<int>(sizeof(rawValue)), &error))
            {
                throw std::runtime_error("readb_string failed");
            }
            return std::string(rawValue);
        }

        const int tagSize = getBinaryTagSize(tag);
        if (tagSize <= 0)
        {
            throw std::runtime_error("不支持的数据类型: " + tag.type + ", tag=" + tag.name);
        }

        // std::array<char, sizeof(std::uint32_t)> rawValue{};
        std::vector<char> rawValue(tagSize);
        if (!readb(ctx.gplatConn, tag.name.c_str(), rawValue.data(), tagSize, &error))
        {
            throw std::runtime_error("readb failed");
        }

        return formatValue(tag, rawValue.data());
    }
    // --------启动时同步读取所有点位当前值到Redis，确保数据一致性---------
    void syncAllTagsToRedis(MonitorPlcDataContext &ctx)
    {
        unsigned int error = 0;
        std::size_t syncedCount = 0;// 统计成功同步的点位数量

        for (const auto &tag : ctx.tags)// 遍历所有点位定义，读取当前值并写入Redis
        {
            try
            {
                error = 0;
                const std::string redisValue = readCurrentValue(ctx, tag, error);
                ctx.redis->set(tag.name, redisValue);
                ++syncedCount;
            }
            catch (const std::exception &ex)
            {
                if (error != 0)
                {
                    spdlog::warn("启动同步读取点位失败: {}, 错误码: {}", tag.name, error);
                }
                else
                {
                    spdlog::error("启动同步写入 Redis 失败: {}, {}", tag.name, ex.what());
                }
            }
        }

        spdlog::info("启动同步 PLC 点位到 Redis 完成: {}/{}", syncedCount, ctx.tags.size());
    }

    // --------连接失败或订阅失败时的重连逻辑---------
    bool reconnectAndResubscribe(MonitorPlcDataContext &ctx, const AppConfig &app, volatile sig_atomic_t &running)
    {
        ctx.Cleanup();
        while (running)
        {
            if (initGplat(ctx) && subscribeAllTags(ctx))
            {
                syncAllTagsToRedis(ctx);// 重连成功后立即同步当前点位值到Redis，确保数据一致性
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

    syncAllTagsToRedis(ctx);// 启动时同步读取所有点位当前值到Redis，确保数据一致性

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