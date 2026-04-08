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

void handleAlignPosOn(TubeTrackContext& ctx,const char* value);

void workThread(TubeTrackContext& ctx)
{
    bool a = true;
    handleAlignPosOn(ctx, reinterpret_cast<const char*>(&a));

   unsigned int err;

    // 订阅timer用于退出检测
    subscribe(ctx.gplatConn, "timer_500ms", &err);
    subscribe(ctx.gplatConn, "ALIGN_POS_ON", &err);

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

        // 处理其他TAG更新
        spdlog::info("Received gPlat update: {} = {}", tagname, value);

        if(tagname == "ALIGN_POS_ON") {
            // 根据控制TAG的值执行相应操作
            handleAlignPosOn(ctx, value);
        }
        else if (tagname == "some_data_tag") {
            // 处理数据TAG更新
        }
        //........
    }
}

void handleAlignPosOn(TubeTrackContext& ctx,const char* value) {
    bool isOn = read_value<bool>(value);
    spdlog::info("isOn: {}", isOn);
    if (isOn) {
        spdlog::info("Align Position ON signal received");
        // 执行对齐工位有料状态的相关操作
        CTube tube;
        if (ctx.prodPlan.Pop(&tube)) {
            ctx.alignPos.Push(tube);
            ctx.alignPos.DebugOut();
        }
    }
}






    // // 初始化生产计划数据
    // ctx.prodPlan.order_no = "20240001";
    // ctx.prodPlan.item_no = "ITEM001";
    // ctx.prodPlan.roll_no = "ROLL1234";
    // ctx.prodPlan.melt_no = "MELT5678";
    // ctx.prodPlan.lot_no = "LOT91011";
    // ctx.prodPlan.lotno_coupling = "COUPLELOT";
    // ctx.prodPlan.meltno_coupling = "COUPLEMELT";
    // ctx.prodPlan.feed_num = 100;  // 初始投料支数
    // ctx.prodPlan.tube_no = 0;     // 初始管号

    // // 初始同步到Redis
    // ctx.prodPlan.UpdateForm();

    // // 模拟生产流程
    // CTube tube;
    // for (int i = 0; i < 100 && g_running; i++)
    // {
    //     // 从投料计划中获取管子数据
    //     if (ctx.prodPlan.Pop(&tube))
    //     {
    //         // 将管子数据推送到测长工位
    //         if (ctx.lengthPos.Push(tube))
    //         {
    //             // 成功推送后，输出工位状态
    //             ctx.lengthPos.DebugOut();

    //             // 从测长工位弹出管子数据
    //             ctx.lengthPos.Pop(&tube);
    //         }

    //         // 更新Redis数据
    //         ctx.prodPlan.UpdateForm();
    //     }

    //     sleep(1); // 模拟生产节奏
    // }