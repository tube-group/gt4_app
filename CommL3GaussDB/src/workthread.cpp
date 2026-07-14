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

void handleREQUEST_ORDER_DATA_CMD(CommL3Context &ctx, const char *value);
void handleMMWE_TQMTIWEA13(CommL3Context &ctx, const char *value);

void workThread(CommL3Context &ctx)
{
    unsigned int err;

    // 订阅timer用于退出检测
    subscribe(ctx.gplatConn, "timer_500ms", &err);
    subscribe(ctx.gplatConn, "REQUEST_ORDER_DATA_CMD", &err);
    subscribe(ctx.gplatConn, "MMWE_TQMTIWEA13", &err);

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
                spdlog::warn("waitpostdata failed, reconnecting gPlat...");
                // 断线重连逻辑
                continue;
            }

            // timer唤醒，仅用于检查g_running
            if (tagname == "timer_500ms")
            {
                spdlog::debug("Timer tick, g_running={}", g_running);
                continue;
            }

            if (tagname == "WAIT_TIMEOUT")
            {
                continue;
            }

            if (tagname == "REQUEST_ORDER_DATA_CMD")
            {            
                // 处理请求订单数据的逻辑
                handleREQUEST_ORDER_DATA_CMD(ctx, value);
            }
            else if (tagname == "MMWE_TQMTIWEA13")
            {
                // 处理MMWE_TQMTIWEA13的逻辑
                handleMMWE_TQMTIWEA13(ctx, value);
            }
            else
            {
                spdlog::warn("Unhandled tag: {}", tagname);
            }
        }
        catch (const std::exception &ex)
        {
            spdlog::error("Error processing tag: {}, error: {}", tagname, ex.what());
            throw; // 继续抛出异常，交由上层处理（可能导致线程退出）
        }
    }
}