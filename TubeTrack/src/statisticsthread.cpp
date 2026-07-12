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
#include "TubeTrackContext.h"

// 声明外部变量
extern volatile sig_atomic_t g_running;

void DoStatistics(TubeTrackContext &ctx);

void StatisticsThread(TubeTrackContext &ctx)
{

    spdlog::debug("StatisticsThread waiting for gPlat data...");
    unsigned int err;

    subscribe(ctx.gplatConn, "timer_1s", &err);

    int loop = 0;
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

            if (tagname == "timer_1s")
            {
                loop++;
                if (loop >= 60)
                {
                    DoStatistics(ctx);
                    loop = 0;
                }
            }
            else if (tagname == "WAIT_TIMEOUT")
            {
                continue;
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
    }
}
