#include <thread>
#include <string>
#include <cstdio>
#include <cstdlib>
#include <atomic>
#include <cstring>
#include <chrono>
#include <unistd.h>
#include <csignal>
#include <utility>

#include "logging.h"
#include "higplat.h"
#include "CommL3Context.h"
#include "usercmd.h"

// 声明外部变量
extern volatile sig_atomic_t g_running;

void workThread(CommL3Context &ctx)
{
    unsigned int err;

    // 订阅timer用于退出检测
    subscribe(ctx.gplatConn, "timer_500ms", &err);

    // 主循环：等待gPlat数据，处理TAG更新
    while (g_running)
    {
        char value[1024] = {0};
        std::string tagname;

        try
        {
            bool ret = waitpostdata(ctx.gplatConn, tagname, value, 1024, -1, &err);

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

            // 处理其他TAG更新
            spdlog::info("Received gPlat post: {}", tagname);
        }
        catch (const std::exception &ex)
        {
            spdlog::error("Error processing tag: {}, error: {}", tagname, ex.what());
            throw; // 继续抛出异常，交由上层处理（可能导致线程退出）
        }
        //.....
    }
}