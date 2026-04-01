#include <thread>
#include <string>
#include <cstdio>
#include <cstdlib>
#include <atomic>
#include <cstring>
#include <chrono>
#include <unistd.h>
#include <csignal>

#include "logging.h"
#include "../include/higplat.h"
#include "TubeTrackContext.h"

// 声明外部变量
extern volatile sig_atomic_t g_running;

void workThread(TubeTrackContext& ctx)
{
    // 初始同步到Redis
    ctx.prodPlan.UpdateForm();

    // 模拟生产流程
    CTube tube;
    for (int i = 0; i < 100 && g_running; i++)
    {
        // 从投料计划中获取管子数据
        if (ctx.prodPlan.Pop(&tube))
        {
            // 将管子数据推送到测长工位
            if (ctx.lengthPos.Push(tube))
            {
                // 成功推送后，输出工位状态
                ctx.lengthPos.DebugOut();

                // 从测长工位弹出管子数据
                ctx.lengthPos.Pop(&tube);
            }

            // 更新Redis数据
            ctx.prodPlan.UpdateForm();
        }

        sleep(1); // 模拟生产节奏
    }

    
    unsigned int err;

    // 订阅timer用于退出检测
    subscribe(ctx.gplatConn, "timer_500ms", &err);

    // 主循环：等待gPlat数据，处理TAG更新
    while (g_running) {
        char value[1024] = {0};
        std::string tagname;

        bool ret = waitpostdata(ctx.gplatConn, tagname, value, 1024, -1, &err);

        if (!ret) {
            spdlog::warn("waitpostdata failed, reconnecting gPlat...");
            // 断线重连逻辑
            continue;
        }

        // timer唤醒，仅用于检查g_running
        if (tagname == "timer_500ms") {
            continue;
        }

        if (tagname == "WAIT_TIMEOUT") {
            continue;
        }
    }
}