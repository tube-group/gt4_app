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
#include "usercmd.h"

// 声明外部变量
extern volatile sig_atomic_t g_running;

void handleAlignPosOn(TubeTrackContext &ctx, const char *value);
void handleWeiPosOn(TubeTrackContext &ctx, const char *value);
void handlePrtPosOn(TubeTrackContext &ctx, const char *value);
void handleSpyPosOn(TubeTrackContext &ctx, const char *value);
void handleCirPosOn(TubeTrackContext &ctx, const char *value);
void handleScrRollerOn(TubeTrackContext &ctx, const char *value);
void handleWbBase(TubeTrackContext &ctx, const char *value);
void moveTubeToWbase(TubeTrackContext &ctx);
void moveTubeToPosion(TubeTrackContext &ctx);
void handleMoveTubeCmd(TubeTrackContext &ctx, const char *value); // 处理移动管子命令

bool moveTubeBetween(CPositionBase &source,
                     CPositionBase &target,
                     const char *sourceName,
                     const char *targetName)
{
    if (!target.IsEmpty())
    {
        spdlog::warn("{} is not empty, cannot move tube from {}", targetName, sourceName);
        return false;
    }

    if (source.IsEmpty())
    {
        spdlog::warn("{} is empty, no tube to move to {}", sourceName, targetName);
        return false;
    }

    auto tube = source.Pop();
    if (!tube)
    {
        spdlog::warn("{} returned no tube, cannot move to {}", sourceName, targetName);
        return false;
    }

    if (!target.Push(std::move(tube)))
    {
        spdlog::error("Failed to push tube from {} to {}", sourceName, targetName);
        return false;
    }

    target.DebugOut();
    return true;
}

void workThread(TubeTrackContext &ctx)
{
    // // // 手工模拟管子的完整流程。

    // // 第 0 步：步进梁在基位
    // bool on = true;
    // handleWbBase(ctx, reinterpret_cast<const char *>(&on));

    // // 第 1 步：投料到对齐工位
    // spdlog::info("第 1 步：投料到对齐工位");
    // handleAlignPosOn(ctx, reinterpret_cast<const char *>(&on));

    // // 第 2 步：对齐工位下料，管子被收集到步进梁 1 号位
    // spdlog::info("第 2 步：对齐工位下料，管子被收集到步进梁 1 号位");
    // bool off = false;
    // handleWbBase(ctx, reinterpret_cast<const char *>(&off));
    // handleAlignPosOn(ctx, reinterpret_cast<const char *>(&off));

    // // // // 第 3 步：投新管 + 分发，步进梁 1 号位的管子下到称重工位
    // // // spdlog::info("第 3 步：投新管，步进梁 1 号位的管子下到称重工位");
    // // // handleAlignPosOn(ctx, reinterpret_cast<const char *>(&on)); // 关键：用对齐信号

    // // 第 3 步：不投新管子+分发，步进梁 1 号位的管子下到称重工位
    // spdlog::info("第 3 步：步进梁 1 号位的管子下到称重工位");
    // handleWbBase(ctx, reinterpret_cast<const char *>(&off));
    // handleWeiPosOn(ctx, reinterpret_cast<const char *>(&on)); // 用称重信号触发分发

    // // 第 4 步：称重工位下料，管子回到步进梁 2 号位
    // spdlog::info("第 4 步：称重工位下料，管子回到步进梁 2 号位");
    // handleWeiPosOn(ctx, reinterpret_cast<const char *>(&off));

    // // // // 第 5 步：投新管 + 分发，步进梁 2 号位的管子下到刻印工位
    // // // spdlog::info("第 5 步：投新管，步进梁 2 号位的管子下到刻印工位");
    // // // handleAlignPosOn(ctx, reinterpret_cast<const char *>(&on));

    // // 第 5 步：分发，步进梁 2 号位的管子下到刻印工位（不投新料）
    // spdlog::info("第 5 步：步进梁 2 号位的管子下到刻印工位");
    // handlePrtPosOn(ctx, reinterpret_cast<const char *>(&on)); // 用刻印信号触发分发

    // // 第 6 步：刻印工位下料，管子回到步进梁 3 号位
    // spdlog::info("第 6 步：刻印工位下料，管子回到步进梁 3 号位");
    // handlePrtPosOn(ctx, reinterpret_cast<const char *>(&off));

    // // // // 第 7 步：投新管 + 分发，步进梁 3 号位的管子下到喷印工位
    // // // spdlog::info("第 7 步：投新管，步进梁 3 号位的管子下到喷印工位");
    // // // handleAlignPosOn(ctx, reinterpret_cast<const char *>(&on));

    // // 第 7 步：分发，步进梁 3 号位的管子下到喷印工位（不投新料）
    // spdlog::info("第 7 步：步进梁 3 号位的管子下到喷印工位");
    // handleSpyPosOn(ctx, reinterpret_cast<const char *>(&on)); // 用喷印信号触发分发

    // // 第 8 步：喷印工位下料，管子回到步进梁 4 号位
    // spdlog::info("第 8 步：喷印工位下料，管子回到步进梁 4 号位");
    // handleSpyPosOn(ctx, reinterpret_cast<const char *>(&off));

    // // // // 第 9 步：投新管 + 分发，步进梁 4 号位的管子下到色环工位
    // // // spdlog::info("第 9 步：投新管，步进梁 4 号位的管子下到色环工位");
    // // // handleAlignPosOn(ctx, reinterpret_cast<const char *>(&on));

    // // 第 9 步：分发，步进梁 4 号位的管子下到色环工位（不投新料）
    // spdlog::info("第 9 步：步进梁 4 号位的管子下到色环工位");
    // handleCirPosOn(ctx, reinterpret_cast<const char *>(&on)); // 用色环信号触发分发

    // // 第 10 步：色环工位下料，管子回到步进梁 5 号位
    // spdlog::info("第 10 步：色环工位下料，管子回到步进梁 5 号位");
    // handleCirPosOn(ctx, reinterpret_cast<const char *>(&off));

    // // // // 第 11 步：投新管 + 分发，步进梁 5 号位的管子下到出料辊道（离开系统）
    // // // spdlog::info("第 11 步：投新管，步进梁 5 号位的管子下到出料辊道");
    // // // handleAlignPosOn(ctx, reinterpret_cast<const char *>(&on));
    // // // handleScrRollerOn(ctx, reinterpret_cast<const char *>(&on));

    // // 第 11 步：分发，步进梁 5 号位的管子下到出料辊道（离开系统）
    // spdlog::info("第 11 步：步进梁 5 号位的管子下到出料辊道");
    // handleScrRollerOn(ctx, reinterpret_cast<const char *>(&on)); // 用出料辊道信号触发分发

    // // 第 12 步：出料辊道下料，管子进入缓冲区
    // spdlog::info("第 12 步：出料辊道下料，管子进入缓冲区");
    // handleScrRollerOn(ctx, reinterpret_cast<const char *>(&off));

    unsigned int err;

    // 订阅timer用于退出检测
    subscribe(ctx.gplatConn, "timer_500ms", &err);
    subscribe(ctx.gplatConn, "ALIGN_POS_ON", &err);
    subscribe(ctx.gplatConn, "WEIGHT_POS_ON", &err);
    subscribe(ctx.gplatConn, "CARVE_POS_ON", &err);
    subscribe(ctx.gplatConn, "SPRAY_POS_ON", &err);
    subscribe(ctx.gplatConn, "CIRCLE_POS_ON", &err);
    subscribe(ctx.gplatConn, "SCRAPTROLLER_POS_ON", &err);
    subscribe(ctx.gplatConn, "WB_BASE", &err);
    subscribe(ctx.gplatConn, "MOVE_TUBE_CMD", &err);

    // 主循环：等待gPlat数据，处理TAG更新
    while (g_running)
    {
        char value[1024] = {0};
        std::string tagname;

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
            continue;
        }

        if (tagname == "WAIT_TIMEOUT")
        {
            continue;
        }

        // 处理其他TAG更新
        spdlog::info("Received gPlat post: {}", tagname);

        if (tagname == "ALIGN_POS_ON")
        {
            // 处理对齐工位检测信号
            handleAlignPosOn(ctx, value);
        }
        else if (tagname == "WEI_POS_ON")
        {
            // 处理称重工位检测信号
            handleWeiPosOn(ctx, value);
        }
        else if (tagname == "PRT_POS_ON")
        {
            // 处理刻印工位检测信号
            handlePrtPosOn(ctx, value);
        }
        else if (tagname == "SPY_POS_ON")
        {
            // 处理喷印工位检测信号
            handleSpyPosOn(ctx, value);
        }
        else if (tagname == "CIR_POS_ON")
        {
            // 处理色环工位检测信号
            handleCirPosOn(ctx, value);
        }
        else if (tagname == "SCR_ROLLER_ON")
        {
            // 处理废料辊道检测信号
            handleScrRollerOn(ctx, value);
        }
        else if (tagname == "WB_BASE")
        {
            // 处理步进梁基位检测信号
            handleWbBase(ctx, value);
        }
        else if (tagname == "MOVE_TUBE_CMD")
        {
            // 处理移动管子命令
            handleMoveTubeCmd(ctx, value);
        }
    }
}

// 移动管子命令
// plan:投料虚拟工位
// align:对齐工位
// weight：称重工位
// carve:刻印工位
// spray:喷码工位
// circle:色环工位
// scraptroller:出废辊道工位
// scrapt:废料台架工位
// backbuffer:打包前缓冲区工位
// basket:打包区工位

void handleMoveTubeCmd(TubeTrackContext &ctx, const char *value)
{
    MoveTubeCmd cmd = read_value<MoveTubeCmd>(value);
    spdlog::info("Handling MOVE_TUBE_CMD: from={}, to={}", cmd.from.c_str(), cmd.to.c_str());

    if (cmd.from == "plan" && cmd.to == "align") // 生产计划 -> 对齐工位
    {
        if (!ctx.alignPos.IsEmpty())
        {
            spdlog::warn("Align position is not empty, cannot move tube from Production plan");
            return;
        }

        if (ctx.prodPlan.IsEmpty())
        {
            spdlog::warn("Production plan is empty, no tube to move to Align position");
            return;
        }

        auto tube = ctx.prodPlan.Pop();
        if (!tube)
        {
            spdlog::warn("Production plan returned no tube, cannot move to Align position");
            return;
        }

        if (!ctx.alignPos.Push(std::move(tube)))
        {
            spdlog::error("Failed to push tube from Production plan to Align position");
            return;
        }

        ctx.alignPos.DebugOut();
    }
    else if (cmd.from == "align" && cmd.to == "plan") // 反向：对齐工位 -> 生产计划
    {
        // 判断生产计划是否允许回退管子
    }
    else if (cmd.from == "align" && cmd.to == "weight") // 对齐工位 -> 称重工位
    {
        moveTubeBetween(ctx.alignPos, ctx.weightPos, "Align position", "Weight position");
    }
    else if (cmd.from == "weight" && cmd.to == "align") // 反向：称重工位 -> 对齐工位
    {
        moveTubeBetween(ctx.weightPos, ctx.alignPos, "Weight position", "Align position");
    }
    else if (cmd.from == "weight" && cmd.to == "carve") // 称重工位 -> 刻印工位
    {
        moveTubeBetween(ctx.weightPos, ctx.carvePos, "Weight position", "Carve position");
    }
    else if (cmd.from == "carve" && cmd.to == "weight") // 反向：刻印工位 -> 称重工位
    {
        moveTubeBetween(ctx.carvePos, ctx.weightPos, "Carve position", "Weight position");
    }
    else if (cmd.from == "carve" && cmd.to == "spray") // 刻印工位 -> 喷印工位
    {
        moveTubeBetween(ctx.carvePos, ctx.sprayPos, "Carve position", "Spray position");
    }
    else if (cmd.from == "spray" && cmd.to == "carve") // 反向：喷印工位 -> 刻印工位
    {
        moveTubeBetween(ctx.sprayPos, ctx.carvePos, "Spray position", "Carve position");
    }
    else if (cmd.from == "spray" && cmd.to == "circle") // 喷印工位 -> 色环工位
    {
        moveTubeBetween(ctx.sprayPos, ctx.circlePos, "Spray position", "Circle position");
    }
    else if (cmd.from == "circle" && cmd.to == "spray") // 反向：色环工位 -> 喷印工位
    {
        moveTubeBetween(ctx.circlePos, ctx.sprayPos, "Circle position", "Spray position");
    }
    else if (cmd.from == "circle" && cmd.to == "scraptroller") // 色环工位 -> 废料辊道
    {
        moveTubeBetween(ctx.circlePos, ctx.scraptRoller, "Circle position", "Scrapt roller");
    }
    else if (cmd.from == "scraptroller" && cmd.to == "circle") // 反向：废料辊道 -> 色环工位
    {
        moveTubeBetween(ctx.scraptRoller, ctx.circlePos, "Scrapt roller", "Circle position");
    }
    else if (cmd.from == "scraptroller" && cmd.to == "backbuffer") // 废料辊道 -> 缓冲区
    {
        // 缓冲区是多管子的，直接推送即可，无需判断是否有管子
        // 判断废料辊道是否有管子
        if (ctx.scraptRoller.IsEmpty())
        {
            spdlog::warn("Scrapt roller is empty, no tube to move to back buffer");
            return;
        }
        else
        {
            auto tube = ctx.scraptRoller.Pop();
            if (!tube)
            {
                spdlog::warn("Scrapt roller is empty, no tube to move to back buffer");
            }
            else if (!ctx.backBuffer.Push(std::move(tube)))
            {
                spdlog::error("Failed to push tube from Scrapt roller to back buffer");
            }
            else
            {
                ctx.backBuffer.DebugOut();
            }
        }
    }
    // 问题在上游发出的 MOVE_TUBE_CMD 参数不包含这个命令，暂时注释掉
    // （需要修正 gPlat 或命令发送端，把 scaptroller 改成 scraptroller）
    else if (cmd.from == "backbuffer" && cmd.to == "scraptroller")  // 反向：缓冲区 -> 废料辊道
    {
        moveTubeBetween(ctx.backBuffer, ctx.scraptRoller, "Back buffer", "Scrapt roller");
    }
    // 这里画面按钮还没定义命令
    // else if (cmd.from == "scraptroller" && cmd.to == "scrapt")  // 废料辊道 -> 废料台架
    // {
    //     moveTubeBetween(ctx.scraptRoller, ctx.scrapt, "Scrapt roller", "Scrapt");
    // }else if (cmd.from == "scrapt" && cmd.to == "scraptroller")  // 反向：废料台架 -> 废料辊道
    // {
    //     moveTubeBetween(ctx.scrapt, ctx.scraptRoller, "Scrapt", "Scrapt roller");
    // }
    else if (cmd.from == "backbuffer" && cmd.to == "basket") // 缓冲区 -> 打包区(先进先出)
    {
        // 打包区是多管子的，直接推送即可，无需判断是否有管子
        // 判断缓冲区是否有管子
        if (ctx.backBuffer.IsEmpty())
        {
            spdlog::warn("Back buffer is empty, no tube to move to basket");
            return;
        }
        else
        {
            auto tube = ctx.backBuffer.Pop(); // PopFront() - 取出最早进入的
            if (!tube)
            {
                spdlog::warn("Back buffer is empty, no tube to move to basket");
            }
            else if (!ctx.basket.Push(std::move(tube))) // PushBack() - 追加到末尾
            {
                spdlog::error("Failed to push tube from back buffer to basket");
            }
            else
            {
                ctx.basket.DebugOut();
            }
        }
    }
    else if (cmd.from == "basket" && cmd.to == "backbuffer") // 反向：打包区 -> 缓冲区（后进先出）
    {
        // 缓冲区是多管子的，直接推送即可，无需判断是否有管子
        // 判断打包区是否有管子
        if (ctx.basket.IsEmpty())
        {
            spdlog::warn("Basket is empty, no tube to move to back buffer");
            return;
        }
        else
        {
            auto tube = ctx.basket.PopBack(); // PopBack() - 取出最后进入的
            if (!tube)
            {
                spdlog::warn("Basket is empty, no tube to move to back buffer");
            }
            else if (!ctx.backBuffer.PushFront(std::move(tube))) //  PushFront() - 插入到最前面
            {
                spdlog::error("Failed to push tube from basket to back buffer");
            }
            else
            {
                ctx.backBuffer.DebugOut();
            }
        }
    }
    else
    {
        spdlog::warn("Unsupported MOVE_TUBE_CMD: from={}, to={}", cmd.from.c_str(), cmd.to.c_str());
    }
}

//--------将对齐、称重、刻印、喷印、色环工位的管子弹出到步进梁--------
void moveTubeToWbase(TubeTrackContext &ctx)
{
    if (ctx.alignPos.IsEmpty() && ctx.weightPos.IsEmpty() && ctx.carvePos.IsEmpty() && ctx.sprayPos.IsEmpty() && ctx.circlePos.IsEmpty())
    {
        spdlog::warn("All positions are empty, no tubes to move to walking beam");
        return;
    }
    // 从对齐、称重、刻印、喷印、色环工位弹出管子，推送到步进梁
    auto tube1 = ctx.alignPos.Pop();
    auto tube2 = ctx.weightPos.Pop();
    auto tube3 = ctx.carvePos.Pop();
    auto tube4 = ctx.sprayPos.Pop();
    auto tube5 = ctx.circlePos.Pop();

    if (ctx.walkingBeam.Push(std::move(tube1), std::move(tube2), std::move(tube3), std::move(tube4), std::move(tube5)))
    {
        spdlog::info("Moved tubes to walking beam");
        ctx.walkingBeam.DebugOut();
    }
    else
    {
        spdlog::error("Failed to move tubes to walking beam");
    }
}

//--------从步进梁弹出管子，推送到对齐、称重、刻印、喷印、色环工位--------
void moveTubeToPosion(TubeTrackContext &ctx)
{
    if (ctx.walkingBeam.IsEmpty())
    {
        spdlog::warn("Walking beam is empty, no tubes to move to positions");
        return;
    }

    // 从步进梁弹出管子，推送到对齐、称重、刻印、喷印、色环、废料辊道工位
    ctx.weightPos.Push(ctx.walkingBeam.Pop(1));
    ctx.carvePos.Push(ctx.walkingBeam.Pop(2));
    ctx.sprayPos.Push(ctx.walkingBeam.Pop(3));
    ctx.circlePos.Push(ctx.walkingBeam.Pop(4));
    ctx.scraptRoller.Push(ctx.walkingBeam.Pop(5));
}

//--------处理对齐信号ALIGN_POS_ON--------
void handleAlignPosOn(TubeTrackContext &ctx, const char *value)
{
    bool isOn = read_value<bool>(value);
    spdlog::info("ALIGN_POS_ON isOn: {}", isOn);

    if (isOn)
    {
        // 执行对齐工位有料状态的相关操作
        auto tube = ctx.prodPlan.Pop();
        if (tube && ctx.alignPos.Push(std::move(tube)))
        {
            ctx.alignPos.DebugOut();
        }
        else if (!tube)
        {
            spdlog::warn("Production plan is empty, no tube to move to align position");
        }
        else
        {
            spdlog::error("Failed to push tube into align position");
        }

        // 从步进梁弹出管子，推送到称重、刻印、喷印、色环，出废辊道工位
        moveTubeToPosion(ctx);
    }
    else
    {
        if (ctx.walkingBeam.IsAtBase())
        {
            spdlog::warn("Walking beam is at base, ignoring align position signal");
            return;
        }
        // 将对齐、称重、刻印、喷印、色环工位的管子弹出到步进梁
        moveTubeToWbase(ctx);
    }
}

//--------处理称重信号WEIGHT_POS_ON--------
void handleWeiPosOn(TubeTrackContext &ctx, const char *value)
{
    bool isOn = read_value<bool>(value);
    spdlog::info("WEI_POS_ON isOn: {}", isOn);
    if (ctx.walkingBeam.IsAtBase())
    {
        spdlog::warn("Walking beam is at base, ignoring weight position signal");
        return;
    }
    if (!isOn)
    {
        // 将对齐、称重、刻印、喷印、色环工位的管子弹出到步进梁
        moveTubeToWbase(ctx);
    }
    else
    {
        // 从步进梁弹出管子，推送到称重、刻印、喷印、色环，出废辊道工位
        moveTubeToPosion(ctx);
    }
}

//--------处理刻印信号PRT_POS_ON--------
void handlePrtPosOn(TubeTrackContext &ctx, const char *value)
{
    bool isOn = read_value<bool>(value);
    spdlog::info("PRT_POS_ON isOn: {}", isOn);
    if (ctx.walkingBeam.IsAtBase())
    {
        spdlog::warn("Walking beam is at base, ignoring carve position signal");
        return;
    }
    if (!isOn)
    {
        // 将对齐、称重、刻印、喷印、色环工位的管子弹出到步进梁
        moveTubeToWbase(ctx);
    }
    else
    {
        // 从步进梁弹出管子，推送到称重、刻印、喷印、色环，出废辊道工位
        moveTubeToPosion(ctx);
    }
}

//--------处理喷印信号SPY_POS_ON--------
void handleSpyPosOn(TubeTrackContext &ctx, const char *value)
{
    bool isOn = read_value<bool>(value);
    spdlog::info("SPY_POS_ON isOn: {}", isOn);
    if (ctx.walkingBeam.IsAtBase())
    {
        spdlog::warn("Walking beam is at base, ignoring spray position signal");
        return;
    }
    if (!isOn)
    {
        // 将对齐、称重、刻印、喷印、色环工位的管子弹出到步进梁
        moveTubeToWbase(ctx);
    }
    else
    {
        // 从步进梁弹出管子，推送到称重、刻印、喷印、色环，出废辊道工位
        moveTubeToPosion(ctx);
    }
}

//--------处理色环信号CIR_POS_ON--------
void handleCirPosOn(TubeTrackContext &ctx, const char *value)
{
    bool isOn = read_value<bool>(value);
    spdlog::info("CIR_POS_ON isOn: {}", isOn);
    if (ctx.walkingBeam.IsAtBase())
    {
        spdlog::warn("Walking beam is at base, ignoring circle position signal");
        return;
    }
    if (!isOn)
    {
        // 将对齐、称重、刻印、喷印、色环工位的管子弹出到步进梁
        moveTubeToWbase(ctx);
    }
    else
    {
        // 从步进梁弹出管子，推送到称重、刻印、喷印、色环，出废辊道工位
        moveTubeToPosion(ctx);
    }
}

//--------处理废料辊道信号SCR_ROLLER_ON--------
void handleScrRollerOn(TubeTrackContext &ctx, const char *value)
{
    bool isOn = read_value<bool>(value);
    spdlog::info("SCR_ROLLER_ON isOn: {}", isOn);
    if (!isOn)
    {
        // 从废料辊道弹出管子，推送到缓冲区
        auto tube = ctx.scraptRoller.Pop();
        if (tube && ctx.backBuffer.Push(std::move(tube)))
        {
            ctx.backBuffer.DebugOut();
        }
        else if (!tube)
        {
            spdlog::warn("Scrap roller is empty, no tube to move to back buffer");
        }
        else
        {
            spdlog::error("Failed to push tube into back buffer");
        }
    }
    else
    {
        if (ctx.walkingBeam.IsAtBase())
        {
            spdlog::warn("Walking beam is at base, ignoring scrap roller signal");
            return;
        }
        // 从步进梁弹出管子，推送到称重、刻印、喷印、色环，出废辊道工位
        moveTubeToPosion(ctx);
    }
}

//--------处理步进梁基位信号WB_BASE--------
void handleWbBase(TubeTrackContext &ctx, const char *value)
{
    bool isOn = read_value<bool>(value);
    spdlog::info("WB_BASE isOn: {}", isOn);

    ctx.walkingBeam.SetAtBase(isOn);
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
// for (int i = 0; i < 100 && g_running; i++)
// {
//     // 从投料计划中获取管子数据
//     auto tube = ctx.prodPlan.Pop();
//     if (tube)
//     {
//         // 将管子数据推送到测长工位
//         if (ctx.lengthPos.Push(std::move(tube)))
//         {
//             // 成功推送后，输出工位状态
//             ctx.lengthPos.DebugOut();

//             // 从测长工位弹出管子数据
//             tube = ctx.lengthPos.Pop();
//         }

//         // 更新Redis数据
//         ctx.prodPlan.UpdateForm();
//     }

//     sleep(1); // 模拟生产节奏
// }

// else if (cmd.from == "scrapt" && cmd.to == "basket") // 从废料辊道移动到篮子
// {
//     // 判断篮子是否有管子，如果没有再从废料辊道弹出
//     if (ctx.basket.IsEmpty())
//     {
//         // 判断废料辊道是否有管子，如果有再推送到篮子
//         if (!ctx.scraptRoller.IsEmpty())
//         {
//             auto tube = ctx.scraptRoller.Pop();
//             if (tube && ctx.basket.Push(std::move(tube)))
//             {
//                 ctx.basket.DebugOut();
//             }
//             else if (!tube)
//             {
//                 spdlog::warn("Scrapt roller is empty, no tube to move to basket");
//             }
//             else
//             {
//                 spdlog::error("Failed to push tube into basket");
//             }
//         }
//         else
//         {
//             spdlog::warn("Scrapt roller is empty, no tube to move to basket");
//         }
//     }
//     else
//     {
//         spdlog::warn("Basket is not empty, cannot move tube from scrapt roller");
//     }
// }