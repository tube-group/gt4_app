#include <thread>
#include <string>
#include <cstdio>
#include <cstdlib>
#include <atomic>
#include <cmath>
#include <cstring>
#include <chrono>
#include <limits>
#include <stdexcept>
#include <string_view>
#include <unistd.h>
#include <csignal>
#include <utility>

#include "logging.h"
#include "higplat.h"
#include "CommL3Context.h"
#include "user_types.h"

// 声明外部变量
extern volatile sig_atomic_t g_running;

void handleApiOrderData(CommL3Context &ctx, const char *value);
void handleApiBundleDataEvent(CommL3Context &ctx, const char *value);

void workThread(CommL3Context &ctx)
{
    unsigned int err;

    // 订阅timer用于退出检测
    subscribe(ctx.gplatConn, "timer_500ms", &err);
    subscribe(ctx.gplatConn, "API_ORDER_DATA_T", &err);
    subscribe(ctx.gplatConn, "API_BUNDLE_DATA_EVENT", &err);

    // 主循环：等待gPlat数据，处理TAG更新
    while (g_running)
    {
        char value[5000] = {0};
        std::string tagname;

        try
        {
            bool ret = waitpostdata(ctx.gplatConn, tagname, value, 5000, -1, &err);

            if (!ret)
            {
                spdlog::warn("waitpostdata failed, err={}", err);
                // 断线重连逻辑
                sleep(1);
                continue;
            }

            // timer唤醒，仅用于检查g_running
            if (tagname == "timer_500ms")
            {
                spdlog::debug("Timer tick, g_running={}", g_running);
                continue;
            }

            spdlog::info("Received gPlat post: {}", tagname);

            if (tagname == "WAIT_TIMEOUT")
            {
                continue;
            }

            if (tagname == "API_ORDER_DATA_T")
            {            
                // 处理API_ORDER_DATA_T的逻辑
                handleApiOrderData(ctx, value);
            }
            else if (tagname == "API_BUNDLE_DATA_EVENT")
            {
                // 处理API_BUNDLE_DATA_EVENT的逻辑
                handleApiBundleDataEvent(ctx, value);
            }
            else
            {
                spdlog::warn("Unhandled tag: {}, value: {}", tagname, value);
            }
        }
        catch (const std::exception &ex)
        {
            spdlog::error("Error processing tag: {}, error: {}", tagname, ex.what());
            throw; // 继续抛出异常，交由上层处理（可能导致线程退出）
        }
        //.....
    }
}