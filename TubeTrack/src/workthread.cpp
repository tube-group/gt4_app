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
#include "user_types.h"
#include "WeightPosition.h"
#include "DoStatistics.h"
#include "CalculateShift.h"

// 声明外部变量
extern volatile sig_atomic_t g_running;

void handleTask500MS(TubeTrackContext &ctx); // 处理500ms定时任务
void handleAlignPosOn(TubeTrackContext &ctx, const char *value);
void handleWeiPosOn(TubeTrackContext &ctx, const char *value);
void handlePrtPosOn(TubeTrackContext &ctx, const char *value);
void handleSpyPosOn(TubeTrackContext &ctx, const char *value);
void handleSpyPosOnDelay(TubeTrackContext &ctx, const char *value);
void handleCirPosOn(TubeTrackContext &ctx, const char *value);
void handleScrRollerOn(TubeTrackContext &ctx, const char *value);
void handleWbBase(TubeTrackContext &ctx, const char *value);
void moveTubeToWbase(TubeTrackContext &ctx);
void moveTubeToPosion(TubeTrackContext &ctx);
void handleMoveTubeCmd(TubeTrackContext &ctx, const char *value);           // 处理移动管子命令
void executeMoveTubeCmd(TubeTrackContext &ctx, const MoveTubeCmd &cmd);      // 执行移动管子命令
void handleModifyTubeCmd(TubeTrackContext &ctx, const char *value);         // 处理修改管子命令
void handleDeleteTubeCmd(TubeTrackContext &ctx, const char *value);         // 处理删除管子命令
void handleSetCurrentContractCmd(TubeTrackContext &ctx, const char *value); // 处理设置当前合同命令
void handleAddTubeCmd(TubeTrackContext &ctx, const char *value);            // 处理添加管子命令
void autoBundle(TubeTrackContext &ctx);                                     // 自动打捆处理
void handleWeiPosOnDelay(TubeTrackContext &ctx, const char *value); // 处理称重工位延时触发

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

    target.DisableTrigger();
    if (!target.Push(std::move(tube)))
    {
        spdlog::error("Failed to push tube from {} to {}", sourceName, targetName);
        target.EnableTrigger();
        return false;
    }

    target.EnableTrigger();
    target.DebugOut();
    return true;
}

void workThread(TubeTrackContext &ctx)
{
    unsigned int err = 0;

    // 订阅timer用于退出检测
    subscribe(ctx.gplatConn, "timer_500ms", &err);
    subscribe(ctx.gplatConn, "timer_1s", &err);
    subscribe(ctx.gplatConn, "ALIGN_POS_ON", &err);
    subscribe(ctx.gplatConn, "WEIGHT_POS_ON", &err);
    subscribedelaypost(ctx.gplatConn, "WEIGHT_POS_ON", "WEIGHT_POS_ON_DELAY", 2500, &err);
    subscribe(ctx.gplatConn, "CARVE_POS_ON", &err);
    subscribe(ctx.gplatConn, "SPRAY_POS_ON", &err);
    subscribedelaypost(ctx.gplatConn, "SPRAY_POS_ON", "SPRAY_POS_ON_DELAY", 5000, &err);
    subscribe(ctx.gplatConn, "CIRCLE_POS_ON", &err);
    subscribe(ctx.gplatConn, "SCRAPTROLLER_POS_ON", &err);
    subscribe(ctx.gplatConn, "WB_BASE", &err);
    subscribe(ctx.gplatConn, "MOVE_TUBE_CMD", &err);
    subscribe(ctx.gplatConn, "MODIFY_TUBE_CMD", &err);
    subscribe(ctx.gplatConn, "DELETE_TUBE_CMD", &err);
    subscribe(ctx.gplatConn, "SET_CURRENT_CONTRACT_CMD", &err);
    subscribe(ctx.gplatConn, "ADD_TUBE_CMD", &err);
    subscribe(ctx.gplatConn, "FINISH_WEIGHT_EVENT", &err);
    subscribe(ctx.gplatConn, "LENGTH_FINISH", &err); // 订阅测长完成事件
    // subscribe(ctx.gplatConn, "L2_WB_RELEASE", &err); //MonitorUserCmd已处理
    subscribe(ctx.gplatConn, "RELEASE_ALL_POS_CMD", &err);
    subscribe(ctx.gplatConn, "PARAMETER_SET_UPDATED", &err); // 订阅参数集更新事件
    subscribe(ctx.gplatConn, "MANUAL_SPRAY_CMD", &err);
    subscribe(ctx.gplatConn, "MANUAL_CARVE_CMD", &err);
    subscribe(ctx.gplatConn, "MANUAL_LENGTH_CMD", &err);
    subscribe(ctx.gplatConn, "BUNDLE_CMD", &err);            // 订阅打捆命令
    subscribe(ctx.gplatConn, "STAMP_FINISH", &err);          // 订阅刻印完成命令
    subscribe(ctx.gplatConn, "STAMP_START", &err);
    subscribedelaypost(ctx.gplatConn, "STAMP_START", "STAMP_START_DELAY", 3000, &err);
    subscribe(ctx.gplatConn, "LENGTH_START", &err);
    subscribedelaypost(ctx.gplatConn, "LENGTH_START", "LENGTH_START_DELAY", 5000, &err);
    subscribe(ctx.gplatConn, "QUICK_MARK_START", &err);
    subscribedelaypost(ctx.gplatConn, "QUICK_MARK_START", "QUICK_MARK_START_DELAY", 3000, &err);
    subscribe(ctx.gplatConn, "SPRAY_START", &err);
    subscribedelaypost(ctx.gplatConn, "SPRAY_START", "SPRAY_START_DELAY", 3000, &err);
    subscribe(ctx.gplatConn, "SPRAY_FINISH", &err); // 订阅喷印完成事件
    subscribedelaypost(ctx.gplatConn, "SPRAY_FINISH", "SPRAY_FINISH_DELAY", 3000, &err);

    // 复位发给PLC的命令信号
    write_plc_bool(ctx.gplatConn, "SPRAY_START_NOUSE", false, &err);
    write_plc_bool(ctx.gplatConn, "STAMP_START", false, &err);
    write_plc_bool(ctx.gplatConn, "LENGTH_START", false, &err);
    write_plc_bool(ctx.gplatConn, "QUICK_MARK_START", false, &err);
    write_plc_bool(ctx.gplatConn, "SPRAY_START", false, &err);

    // DoStatistics(ctx);

    int loop = 0; // 用于统计1分钟的循环次数
    // 主循环：等待gPlat数据，处理TAG更新
    while (g_running)
    {
        char value[2048] = {0};
        std::string tagname;

        try
        {
            bool ret = waitpostdata(ctx.gplatConn, tagname, value, 1024, -1, &err);

            if (!ret)
            {
                spdlog::warn("waitpostdata failed; retrying gPlat receive, tagname={}, err={}", tagname, err);
                sleep(1);
                continue;
            }

            // timer唤醒，仅用于检查g_running
            if (tagname == "timer_500ms")
            {
                handleTask500MS(ctx);
                continue;
            }

            if (tagname == "timer_1s")
            {
                loop++;
                if (loop >= 60)
                {
                    DoStatistics(ctx);
                    // spdlog::info("开始自动打捆处理");
                    autoBundle(ctx); // 自动打捆处理
                    // spdlog::info("自动打捆处理完成");
                    loop = 0;
                }
                continue;
            }

            if (tagname == "WAIT_TIMEOUT")
            {
                continue;
            }

            // 处理其他TAG更新
            spdlog::info("Received gPlat post: {}", tagname);

            if (tagname == "STAMP_START")
            {
                bool val = read_value<bool>(value);
                spdlog::info("STAMP_START value changed to {}", val);
            }
            else if (tagname == "STAMP_START_DELAY")
            {
                bool val = read_value<bool>(value);
                if (val)
                {
                    spdlog::info("STAMP_START_DELAY triggered, reset STAMP_START");
                    write_plc_bool(ctx.gplatConn, "STAMP_START", false, &err);
                }
            }
            else if (tagname == "LENGTH_START")
            {
                bool val = read_value<bool>(value);
                spdlog::info("LENGTH_START value changed to {}", val);
            }
            else if (tagname == "LENGTH_START_DELAY")
            {
                bool val = read_value<bool>(value);
                if (val)
                {
                    spdlog::info("LENGTH_START_DELAY triggered, reset LENGTH_START");
                    write_plc_bool(ctx.gplatConn, "LENGTH_START", false, &err);
                }
            }
            else if (tagname == "MANUAL_LENGTH_CMD")
            {
                spdlog::info("MANUAL_LENGTH_CMD received, triggering LENGTH_START");
                unsigned int err = 0;
                 write_plc_bool(ctx.gplatConn, "LENGTH_START", true, &err);
            }
            else if (tagname == "QUICK_MARK_START")
            {
                bool val = read_value<bool>(value);
                spdlog::info("QUICK_MARK_START value changed to {}", val);
            }
            else if (tagname == "QUICK_MARK_START_DELAY")
            {
                bool val = read_value<bool>(value);
                if (val)
                {
                    spdlog::info("QUICK_MARK_START_DELAY triggered, reset QUICK_MARK_START");
                    write_plc_bool(ctx.gplatConn, "QUICK_MARK_START", false, &err);
                }
            }
            else if (tagname == "SPRAY_START")
            {
                bool val = read_value<bool>(value);
                spdlog::info("SPRAY_START value changed to {}", val);
            }
            else if (tagname == "SPRAY_START_DELAY")
            {
                bool val = read_value<bool>(value);
                if (val)
                {
                    spdlog::info("SPRAY_START_DELAY triggered, reset SPRAY_START");
                    write_plc_bool(ctx.gplatConn, "SPRAY_START", false, &err);
                }
            }
            else if (tagname == "SPRAY_FINISH")
            {
                bool val = read_value<bool>(value);
                spdlog::info("SPRAY_FINISH value changed to {}", val);
                if (val)
                {
                    ctx.sprayPos.HandleSprayFinish();
                }
            }
            else if (tagname == "SPRAY_FINISH_DELAY")
            {
                bool val = read_value<bool>(value);
                if (val)
                {
                    spdlog::info("SPRAY_FINISH_DELAY triggered, reset SPRAY_START_NOUSE");
                    write_plc_bool(ctx.gplatConn, "SPRAY_START_NOUSE", false, &err);
                }
            }
            else if (tagname == "ALIGN_POS_ON")
            {
                // 处理对齐工位检测信号
                handleAlignPosOn(ctx, value);
            }
            else if (tagname == "WEIGHT_POS_ON")
            {
                // 处理称重工位检测信号
                handleWeiPosOn(ctx, value);
            }
            else if (tagname == "WEIGHT_POS_ON_DELAY")
            {
                // 处理称重工位检测信号延时
                handleWeiPosOnDelay(ctx, value);
            }
            else if (tagname == "CARVE_POS_ON")
            {
                // 处理刻印工位检测信号
                handlePrtPosOn(ctx, value);
            }
            else if (tagname == "SPRAY_POS_ON")
            {
                // 处理喷印工位检测信号
                handleSpyPosOn(ctx, value);
            }
            else if (tagname == "SPRAY_POS_ON_DELAY")
            {
                // 处理喷印工位检测信号延时
                handleSpyPosOnDelay(ctx, value);
            }
            else if (tagname == "CIRCLE_POS_ON")
            {
                // 处理色环工位检测信号
                handleCirPosOn(ctx, value);
            }
            else if (tagname == "SCRAPTROLLER_POS_ON")
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
            else if (tagname == "MODIFY_TUBE_CMD")
            {
                // 处理修改管子命令
                handleModifyTubeCmd(ctx, value);
            }
            else if (tagname == "DELETE_TUBE_CMD")
            {
                // 处理删除管子命令
                handleDeleteTubeCmd(ctx, value);
            }
            else if (tagname == "BUNDLE_CMD")
            {
                // 处理打捆命令
                spdlog::info("Handling BUNDLE_CMD");
                ctx.basket.Bundle();
            }
            else if (tagname == "SET_CURRENT_CONTRACT_CMD")
            {
                // 处理设置当前合同命令
                handleSetCurrentContractCmd(ctx, value);
            }
            else if (tagname == "ADD_TUBE_CMD")
            {
                // 处理添加管子命令
                handleAddTubeCmd(ctx, value);
            }
            else if (tagname == "FINISH_WEIGHT_EVENT")
            {
                // 调用称重工位的SetTubeWeight函数
                spdlog::info("Handling FINISH_WEIGHT_EVENT, setting tube weight in WeightPosition");
                int weight = read_value<int>(value);
                ctx.weightPos.SetTubeWeight(weight);
                // ctx.weightPos.SetTubeWeight(1001);
            }
            else if (tagname == "LENGTH_FINISH")
            {
                // 测长完成，读取测长实际值并交给喷印工位处理
                bool lengthFinish = read_value<bool>(value);
                if (!lengthFinish)
                {
                    continue;
                }

                float meaLen = 0.0f;
                unsigned int err = 0;
                if (!readb(ctx.gplatConn, "MEA_LEN", &meaLen, sizeof(meaLen), &err))
                {
                    spdlog::warn("Read MEA_LEN failed, err={}", err);
                    continue;
                }

                spdlog::info("Handling LENGTH_FINISH: MEA_LEN={}", meaLen);
                ctx.sprayPos.HandleLengthReady(meaLen);

                // 复位启动测长信号
                write_plc_bool(ctx.gplatConn, "LENGTH_START", false, &err);
            }
            // else if (tagname == "L2_WB_RELEASE")    //MonitorUserCmd已处理
            // {
            //     bool l2WbRelease = read_value<bool>(value);
            //     ctx.redis->set("L2_WB_RELEASE", l2WbRelease ? "true" : "false");
            //     ctx.redis->publish("RealDataChanged", "L2_WB_RELEASE");
            // }
            else if (tagname == "RELEASE_ALL_POS_CMD")
            {
                int releaseAllPosCmd = read_value<int>(value);
                spdlog::info("Handling RELEASE_ALL_POS_CMD: value={}", releaseAllPosCmd);
                // 处理释放所有工位命令的逻辑
                ctx.alignPos.ReleaseWB();
                ctx.weightPos.ReleaseWB();
                ctx.carvePos.ReleaseWB();
                ctx.sprayPos.ReleaseWB();
                ctx.circlePos.ReleaseWB();
                ctx.scraptRoller.ReleaseWB();
            }
            else if (tagname == "PARAMETER_SET_UPDATED")
            {
                spdlog::info("Handling PARAMETER_SET_UPDATED event");
                // 重新读取各工位的参数集
                ctx.prodPlan.ReadParameterSet();
                ctx.weightPos.ReadParameterSet();
                ctx.carvePos.ReadParameterSet();
                ctx.sprayPos.ReadParameterSet();
                ctx.circlePos.ReadParameterSet();
                ctx.basket.ReadParameterSet();

                // 刷新各工位的画面
                ctx.prodPlan.UpdateForm();
                ctx.weightPos.UpdateForm();
                ctx.carvePos.UpdateForm();
                ctx.sprayPos.UpdateForm();
                ctx.circlePos.UpdateForm();
                ctx.basket.UpdateForm();
            }
            else if (tagname == "MANUAL_SPRAY_CMD")
            {
                ctx.sprayPos.HandleSprayManual();
            }
            else if (tagname == "MANUAL_CARVE_CMD")
            {
                ctx.carvePos.HandleManualCarve();
            }
            else if (tagname == "STAMP_FINISH")
            {
                bool stampFinish = read_value<bool>(value);
                if (stampFinish)
                {
                    spdlog::info("Handling STAMP_FINISH: value={}", stampFinish);
                    ctx.carvePos.HandleCarveFinish();
                }
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

void handleTask500MS(TubeTrackContext &ctx)
{
    unsigned int err;

    int shiftNo;
    std::string shiftStr;
    CalcShiftNow(shiftStr, shiftNo);
    int shiftNoOld;
    readb(ctx.gplatConn, "SHIFT_NO", &shiftNoOld, sizeof(shiftNoOld), &err);
    if (shiftNo != shiftNoOld)
    {
        spdlog::info("Shift changed: old={}, new={}", shiftNoOld, shiftNo);
        writeb(ctx.gplatConn, "SHIFT_NO", &shiftNo, sizeof(shiftNo), &err);
        writeb_string2(ctx.gplatConn, "SHIFT_NAME", shiftStr, &err);

        ctx.redis->set("SHIFT_NO", std::to_string(shiftNo));
        ctx.redis->set("SHIFT_NAME", shiftStr);
        ctx.redis->publish("RealDataChanged", "SHIFT_NAME");
    }

    bool weightRelease = ctx.weightPos.WbReleased();
    bool sprayRelease = ctx.sprayPos.WbReleased();
    bool carveRelease = ctx.carvePos.WbReleased();

    bool l2WbRelease;
    readb(ctx.gplatConn, "L2_WB_RELEASE", &l2WbRelease, sizeof(l2WbRelease), &err);

    writeb(ctx.gplatConn, "WEIGHT_RELEASE", &weightRelease, sizeof(weightRelease), &err);
    bool weightReleaseRedis = ctx.redis->get("WEIGHT_RELEASE") == "true";
    if (weightReleaseRedis != weightRelease)
    { // 如果Redis中的值与当前状态不一致，则更新Redis，避免重复发布
        ctx.redis->set("WEIGHT_RELEASE", weightRelease ? "true" : "false");
        ctx.redis->publish("RealDataChanged", "WEIGHT_RELEASE");
    }

    writeb(ctx.gplatConn, "SPRAY_RELEASE", &sprayRelease, sizeof(sprayRelease), &err);
    bool sprayReleaseRedis = ctx.redis->get("SPRAY_RELEASE") == "true";
    if (sprayReleaseRedis != sprayRelease)
    { // 如果Redis中的值与当前状态不一致，则更新Redis，避免重复发布
        ctx.redis->set("SPRAY_RELEASE", sprayRelease ? "true" : "false");
        ctx.redis->publish("RealDataChanged", "SPRAY_RELEASE");
    }

    writeb(ctx.gplatConn, "CARVE_RELEASE", &carveRelease, sizeof(carveRelease), &err);
    bool carveReleaseRedis = ctx.redis->get("CARVE_RELEASE") == "true";
    if (carveReleaseRedis != carveRelease)
    { // 如果Redis中的值与当前状态不一致，则更新Redis，避免重复发布
        ctx.redis->set("CARVE_RELEASE", carveRelease ? "true" : "false");
        ctx.redis->publish("RealDataChanged", "CARVE_RELEASE");
    }

    bool wbRelease = weightRelease && sprayRelease && carveRelease && l2WbRelease;

    if (wbRelease)
    {
        if (ctx.prodPlan.Count() == 0)
        {
            // Program.qbdConnection.LogAlarm("yjg4_Alarm", "投料支数为0，禁止释放步进梁，请设置投料支数！", 9);
            spdlog::warn("Production plan count is 0, cannot release WB, please set production plan count!");
            unsigned int err;
            write_plc_bool(ctx.gplatConn, "WB_RELEASE", false, &err);
        }
        else
        {
            unsigned int err;
            write_plc_bool(ctx.gplatConn, "WB_RELEASE", true, &err);
        }
    }
    else
    {
        unsigned int err;
        write_plc_bool(ctx.gplatConn, "WB_RELEASE", false, &err);
    }
}

// ------------处理移动管子命令-----------------
void handleMoveTubeCmd(TubeTrackContext &ctx, const char *value)
{
    MoveTubeCmd cmd = read_value<MoveTubeCmd>(value);
    executeMoveTubeCmd(ctx, cmd);
}

void executeMoveTubeCmd(TubeTrackContext &ctx, const MoveTubeCmd &cmd)
{
    spdlog::info("Handling MOVE_TUBE_CMD: from={}, to={}", cmd.from.c_str(), cmd.to.c_str());

    if (cmd.from == "plan" && cmd.to == "align") // 生产计划 -> 对齐工位
    {
        if (!ctx.alignPos.IsEmpty())
        {
            spdlog::warn("Align position is not empty, cannot move tube to Align position");
            return;
        }

        auto tube = ctx.prodPlan.Pop();
        if (!tube)
        {
            spdlog::warn("Production plan returned no tube, cannot move tube to Align position");
            return;
        }

        ctx.alignPos.Push(std::move(tube));
        ctx.alignPos.DebugOut();
    }
    else if (cmd.from == "align" && cmd.to == "plan") // 反向：对齐工位 -> 生产计划
    {
        auto tube = ctx.alignPos.Pop();
        if (!tube)
        {
            spdlog::warn("Align position is empty, no tube to move to Production plan");
            return;
        }
        ctx.prodPlan.Push(std::move(tube));
        // ctx.prodPlan.DebugOut();
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
        auto tube = ctx.scraptRoller.Pop();

        if (!tube)
        {
            // 判断废料辊道是否有管子
            spdlog::warn("Scrapt roller is empty, no tube to move to back buffer");
            return;
        }
        ctx.backBuffer.Push(std::move(tube));
        ctx.backBuffer.DebugOut();
    }
    else if (cmd.from == "backbuffer" && cmd.to == "scraptroller") // 反向：缓冲区 -> 废料辊道
    {
        if (!ctx.scraptRoller.IsEmpty())
        {
            spdlog::warn("Scrapt roller is not empty, cannot move tube from Back buffer");
            return;
        }

        auto tube = ctx.backBuffer.PopBack();
        if (!tube)
        {
            spdlog::warn("Back buffer is empty, no tube to move to Scrapt roller");
            return;
        }

        ctx.scraptRoller.PushFront(std::move(tube));
        ctx.scraptRoller.DebugOut();
    }
    else if (cmd.from == "scraptroller" && cmd.to == "scrapt") // 废料辊道 -> 废料筐
    {

        auto tube = ctx.scraptRoller.Pop();
        if (!tube)
        {
            // 判断废料辊道是否有管子
            spdlog::warn("Scrapt roller is empty, no tube to move to scrapt");
            return;
        }
        ctx.scrapt.Push(std::move(tube));
        ctx.scrapt.DebugOut();
    }
    else if (cmd.from == "backbuffer" && cmd.to == "basket") // 缓冲区 -> 打包区(先进先出)
    {
        auto tube = ctx.backBuffer.Pop(); // PopFront() - 取出最早进入的

        if (!tube)
        {
            spdlog::warn("Back buffer is empty, no tube to move to basket");
            return;
        }
        ctx.basket.Push(std::move(tube));
        ctx.basket.DebugOut();
    }
    else if (cmd.from == "basket" && cmd.to == "backbuffer") // 反向：打包区 -> 缓冲区（后进先出）
    {

        auto tube = ctx.basket.PopBack(); // PopBack() - 取出最后进入的
        if (!tube)
        {
            spdlog::warn("Basket is empty, no tube to move to back buffer");
            return;
        }
        ctx.backBuffer.PushFront(std::move(tube));
        ctx.backBuffer.DebugOut();
    }
    else
    {
        spdlog::warn("Unsupported MOVE_TUBE_CMD: from={}, to={}", cmd.from.c_str(), cmd.to.c_str());
    }
}

// -----------处理修改管子命令----------
void handleModifyTubeCmd(TubeTrackContext &ctx, const char *value)
{
    ModifyTubeCmd cmd = read_value<ModifyTubeCmd>(value);
    spdlog::info("Handling MODIFY_TUBE_CMD: position={}, seq_no={}, flow_no={}", cmd.position_name.c_str(), cmd.seq_no, cmd.flow_no);

    CPositionBase *position = nullptr;
    if (cmd.position_name == "align")
    {
        position = &ctx.alignPos;
    }
    else if (cmd.position_name == "weight")
    {
        position = &ctx.weightPos;
    }
    else if (cmd.position_name == "carve")
    {
        position = &ctx.carvePos;
    }
    else if (cmd.position_name == "spray")
    {
        position = &ctx.sprayPos;
    }
    else if (cmd.position_name == "circle")
    {
        position = &ctx.circlePos;
    }
    else if (cmd.position_name == "scraptroller")
    {
        position = &ctx.scraptRoller;
    }
    else if (cmd.position_name == "backbuffer")
    {
        position = &ctx.backBuffer;
    }
    else if (cmd.position_name == "scrapt")
    {
        position = &ctx.scrapt;
    }
    else if (cmd.position_name == "basket")
    {
        position = &ctx.basket;
    }

    if (!position)
    {
        spdlog::warn("Unsupported MODIFY_TUBE_CMD position: {}", cmd.position_name.c_str());
        return;
    }

    if (!position->Modify(cmd))
    {
        spdlog::warn("Tube not found for MODIFY_TUBE_CMD: position={}, seq_no={}, flow_no={}", cmd.position_name.c_str(), cmd.seq_no, cmd.flow_no);
        return;
    }

    if (cmd.position_name == "spray")
    {
        const CTube *tube = ctx.sprayPos.Peek();
        if (tube != nullptr)
        {
            bool status = tube->length_ok && tube->weight_ok;
            unsigned int err;
            write_plc_bool(ctx.gplatConn, "SPRAY_WASTE_FLAG", status, &err);
            spdlog::info("Updated SPRAY_WASTE_FLAG to {} based on tube status: length_ok={}, weight_ok={}", status, tube->length_ok, tube->weight_ok);
        }
    }

    position->DebugOut();
}

// -----------处理删除管子命令----------
void handleDeleteTubeCmd(TubeTrackContext &ctx, const char *value)
{
    DeleteTubeCmd cmd = read_value<DeleteTubeCmd>(value);
    spdlog::info("Handling DELETE_TUBE_CMD: position={}, seq_no={}", cmd.position_name.c_str(), cmd.seq_no);

    CPositionBase *position = nullptr;
    // bool isMultiPosition = false;
    if (cmd.position_name == "align")
    {
        position = &ctx.alignPos;
    }
    else if (cmd.position_name == "weight")
    {
        position = &ctx.weightPos;
    }
    else if (cmd.position_name == "carve")
    {
        position = &ctx.carvePos;
    }
    else if (cmd.position_name == "spray")
    {
        position = &ctx.sprayPos;
    }
    else if (cmd.position_name == "circle")
    {
        position = &ctx.circlePos;
    }
    else if (cmd.position_name == "scraptroller")
    {
        position = &ctx.scraptRoller;
    }
    else if (cmd.position_name == "backbuffer")
    {
        position = &ctx.backBuffer;
    }
    else if (cmd.position_name == "scrapt")
    {
        position = &ctx.scrapt;
    }
    else if (cmd.position_name == "basket")
    {
        position = &ctx.basket;
    }

    if (!position)
    {
        spdlog::warn("Unsupported DELETE_TUBE_CMD position: {}", cmd.position_name.c_str());
        return;
    }

    position->Delete(cmd.seq_no);

    position->DebugOut();
}

// -----------处理设置当前合同命令----------
void handleSetCurrentContractCmd(TubeTrackContext &ctx, const char *value)
{
    SetCurrentContractCmd cmd = read_value<SetCurrentContractCmd>(value);
    const std::string orderNo = cmd.order_no.c_str();
    const std::string itemNo = cmd.item_no.c_str();

    spdlog::info("Handling SET_CURRENT_CONTRACT_CMD: order_no={}, item_no={}", orderNo, itemNo);

    if (orderNo.empty() || itemNo.empty())
    {
        spdlog::warn("SET_CURRENT_CONTRACT_CMD missing order_no or item_no");
        return;
    }

    ctx.prodPlan.ApplyCurrentContract(orderNo, itemNo);
}

// -----------处理添加管子命令----------
void handleAddTubeCmd(TubeTrackContext &ctx, const char *value)
{
    AddTubeCmd cmd = read_value<AddTubeCmd>(value);
    spdlog::info("Handling ADD_TUBE_CMD: position={}, seq_no={}", cmd.position_name.c_str(), cmd.seq_no);

    // 在此位置前插入管子，seq_no的值表示插入管子的位置，0表示第一个位置，1表示第二个位置，以此类推，-1代表在末尾添加
    if (cmd.position_name == "backbuffer")
    {
        auto tube = std::make_unique<CTube>();

        // 查找pg数据库，获取管子数据，填充到tube对象中
        pqxx::nontransaction ntx(*ctx.pgConn);
        const pqxx::result result = ntx.exec(
            "SELECT tube_no, order_no, item_no, roll_no, melt_no, lot_no, "
            "lot_no_coupling, melt_no_coupling "
            "FROM parameter_set "
            "LIMIT 1");
        if (result.empty())
        {
            spdlog::warn("No tube data found in database, cannot add tube to back buffer");
            return;
        }
        else
        {
            const auto row = result[0];
            tube->tube_no = row["tube_no"].as<int>();
            tube->order_no = row["order_no"].as<std::string>();
            tube->item_no = row["item_no"].as<std::string>();
            tube->roll_no = row["roll_no"].as<std::string>();
            tube->melt_no = row["melt_no"].as<std::string>();
            tube->lot_no = row["lot_no"].as<std::string>();
            tube->lotno_coupling = row["lot_no_coupling"].as<std::string>();
            tube->meltno_coupling = row["melt_no_coupling"].as<std::string>();

            // 其他字段使用默认值
            tube->flow_no = -1; // 流水号为0,提示需要手动修改
            tube->length = 0.0; // 长度（米）
            tube->weight = 0.0; // 重量（KG）
        }

        ctx.backBuffer.PushAt(std::move(tube), cmd.seq_no);
        ctx.backBuffer.DebugOut();
    }
    else if (cmd.position_name == "basket")
    {
        auto tube = std::make_unique<CTube>();

        // 查找pg数据库，获取管子数据，填充到tube对象中
        pqxx::nontransaction ntx(*ctx.pgConn);
        const pqxx::result result = ntx.exec(
            "SELECT tube_no, order_no, item_no, roll_no, melt_no, lot_no, "
            "lot_no_coupling, melt_no_coupling "
            "FROM parameter_set "
            "LIMIT 1");
        if (result.empty())
        {
            spdlog::warn("No tube data found in database, cannot add tube to back buffer");
            return;
        }
        else
        {
            const auto row = result[0];
            tube->tube_no = row["tube_no"].as<int>();
            tube->order_no = row["order_no"].as<std::string>();
            tube->item_no = row["item_no"].as<std::string>();
            tube->roll_no = row["roll_no"].as<std::string>();
            tube->melt_no = row["melt_no"].as<std::string>();
            tube->lot_no = row["lot_no"].as<std::string>();
            tube->lotno_coupling = row["lot_no_coupling"].as<std::string>();
            tube->meltno_coupling = row["melt_no_coupling"].as<std::string>();

            // 其他字段使用默认值
            tube->flow_no = -1; // 流水号为-1,提示需要手动修改
            tube->length = 0.0; // 长度（米）
            tube->weight = 0.0; // 重量（KG）
        }

        ctx.basket.PushAt(std::move(tube), cmd.seq_no);
        ctx.basket.DebugOut();
    }
    else
    {
        spdlog::warn("Unsupported ADD_TUBE_CMD position: {}", cmd.position_name.c_str());
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
    auto tubeAlign = ctx.alignPos.Pop();
    auto tubeWeight = ctx.weightPos.Pop();
    auto tubeCarve = ctx.carvePos.Pop();
    auto tubeSpray = ctx.sprayPos.Pop();
    auto tubeCircle = ctx.circlePos.Pop();

    // 封锁步进梁
    ctx.carvePos.BlockWB();
    ctx.weightPos.BlockWB();
    if (ctx.sprayPos.IsSprayEnable())
    {
        ctx.sprayPos.BlockWB();
    }

    unsigned int err;
    // 通知PLC出废工位的管子是否为废管
    if (tubeCircle != nullptr)
    {
        ushort status = (tubeCircle->length_ok && tubeCircle->weight_ok) ? (ushort)1 : (ushort)3;
        write_plc_ushort(ctx.gplatConn, "WASTE_ROLLER_FLAG", status, &err);
        spdlog::info("通知PLC出废工位的管子是否为废管: status={}", status);
    }
    else
    {
        write_plc_ushort(ctx.gplatConn, "WASTE_ROLLER_FLAG", (ushort)1, &err);
        spdlog::info("通知PLC出废工位的管子是否为废管: status=1");
    }

    if (tubeSpray != nullptr)
    {
        bool circleStatus = (tubeSpray->length_ok && tubeSpray->weight_ok && ctx.circlePos.IsCircleEnable()) ? true : false;
        write_plc_bool(ctx.gplatConn, "CIRCLE_ENABLE", circleStatus, &err);
        spdlog::info("发出色环允许信号: {}", circleStatus);
    }
    else
    {
        bool circleStatus = ctx.circlePos.IsCircleEnable() ? true : false;
        write_plc_bool(ctx.gplatConn, "CIRCLE_ENABLE", circleStatus, &err);
        spdlog::info("发出色环允许信号: {}", circleStatus);
    }

    if (tubeCarve != nullptr)
    {
        bool sprayStatus = (tubeCarve->length_ok && tubeCarve->weight_ok && ctx.sprayPos.IsSprayEnable()) ? true : false;
        write_plc_bool(ctx.gplatConn, "SPRAY_ENABLE", sprayStatus, &err);
        spdlog::info("发出喷印允许信号: {}", sprayStatus);
    }
    else
    {
        bool sprayStatus = ctx.sprayPos.IsSprayEnable() ? true : false;
        write_plc_bool(ctx.gplatConn, "SPRAY_ENABLE", sprayStatus, &err);
        spdlog::info("发出喷印允许信号: {}", sprayStatus);
    }

    if (tubeWeight != nullptr)
    {
        bool carveStatus = (tubeWeight->length_ok && tubeWeight->weight_ok && ctx.carvePos.IsCarveEnable()) ? true : false;
        write_plc_bool(ctx.gplatConn, "CARVE_ENABLE", carveStatus, &err);
        spdlog::info("发出压印允许信号: {}", carveStatus);
    }
    else
    {
        bool carveStatus = ctx.carvePos.IsCarveEnable() ? true : false;
        write_plc_bool(ctx.gplatConn, "CARVE_ENABLE", carveStatus, &err);
        spdlog::info("发出压印允许信号: {}", carveStatus);
    }

    // 复位启动信号
    write_plc_bool(ctx.gplatConn, "LENGTH_START", false, &err);     // 启动测长
    write_plc_bool(ctx.gplatConn, "SPRAY_START", false, &err);      // 启动管体喷印
    write_plc_bool(ctx.gplatConn, "QUICK_MARK_START", false, &err); // 启动条码喷印

    // 复位喷印工位废管标志
    write_plc_bool(ctx.gplatConn, "SPRAY_WASTE_FLAG", false, &err); // 喷印工位废管标志

    ctx.walkingBeam.Push(std::move(tubeAlign), std::move(tubeWeight), std::move(tubeCarve), std::move(tubeSpray), std::move(tubeCircle));
    ctx.walkingBeam.DebugOut();
}

//--------从步进梁弹出管子，推送到称重、刻印、喷印、色环、废料辊道工位工位--------
void moveTubeToPosion(TubeTrackContext &ctx)
{
    if (ctx.walkingBeam.IsEmpty())
    {
        spdlog::warn("Walking beam is empty, no tubes to move to positions");
        return;
    }

    if (!ctx.scraptRoller.IsEmpty())
    {
        spdlog::warn("Scrapt roller is not empty, cannot move tubes to positions");
        // mark
        ctx.scraptRoller.DebugOut();
        // ctx.scraptRoller.Clear();
    }

    // if (ctx.walkingBeam.IsPositionEmpty(2))
    // {
    //     //刻印工位无管，释放步进梁
    //     ctx.carvePos.ReleaseWB();
    //     spdlog::warn("Carve position is empty, releasing walking beam");
    //     return;
    // }

    // 从步进梁弹出管子，推送到称重、刻印、喷印、色环、废料辊道工位
    ctx.weightPos.Push(ctx.walkingBeam.Pop(1));
    ctx.carvePos.Push(ctx.walkingBeam.Pop(2));
    ctx.sprayPos.Push(ctx.walkingBeam.Pop(3));
    ctx.circlePos.Push(ctx.walkingBeam.Pop(4));
    ctx.scraptRoller.Push(ctx.walkingBeam.Pop(5));

    // mark 称重工位无管，释放称重工位(如果实物有管呢)
    if (ctx.weightPos.IsEmpty())
    {
        ctx.weightPos.ReleaseWB();
    }

    if (ctx.carvePos.IsEmpty())
    {
        ctx.carvePos.ReleaseWB();
    }

    if (ctx.sprayPos.IsEmpty())
    {
        ctx.sprayPos.ReleaseWB();
    }

    unsigned int err;
    // 通知PLC出废工位的管子是否为废管
    auto scrapTube = ctx.scraptRoller.Peek();
    if (scrapTube != nullptr)
    {
        ushort status = (scrapTube->length_ok && scrapTube->weight_ok) ? (ushort)1 : (ushort)3;
        write_plc_ushort(ctx.gplatConn, "WASTE_ROLLER_FLAG", status, &err);
        spdlog::info("通知PLC出废工位的管子是否为废管: status={}", status);
    }
    else
    {
        write_plc_ushort(ctx.gplatConn, "WASTE_ROLLER_FLAG", (ushort)1, &err);
        spdlog::info("通知PLC出废工位的管子是否为废管: status=1");
    }

    auto circleTube = ctx.circlePos.Peek();
    if (circleTube != nullptr)
    {
        bool circleStatus = (circleTube->length_ok && circleTube->weight_ok && ctx.circlePos.IsCircleEnable()) ? true : false;
        write_plc_bool(ctx.gplatConn, "CIRCLE_ENABLE", circleStatus, &err);
        spdlog::info("发出色环允许信号: {}", circleStatus);
    }
    else
    {
        bool circleStatus = ctx.circlePos.IsCircleEnable() ? true : false;
        write_plc_bool(ctx.gplatConn, "CIRCLE_ENABLE", circleStatus, &err);
        spdlog::info("发出色环允许信号: {}", circleStatus);
    }

    auto sprayTube = ctx.sprayPos.Peek();
    if (sprayTube != nullptr)
    {
        bool sprayStatus = (sprayTube->length_ok && sprayTube->weight_ok && ctx.sprayPos.IsSprayEnable()) ? true : false;
        write_plc_bool(ctx.gplatConn, "SPRAY_ENABLE", sprayStatus, &err);
        spdlog::info("发出喷印允许信号: {}", sprayStatus);
    }
    else
    {
        bool sprayStatus = ctx.sprayPos.IsSprayEnable() ? true : false;
        write_plc_bool(ctx.gplatConn, "SPRAY_ENABLE", sprayStatus, &err);
        spdlog::info("发出喷印允许信号: {}", sprayStatus);
    }

    auto carveTube = ctx.carvePos.Peek();
    if (carveTube != nullptr)
    {
        bool carveStatus = (carveTube->length_ok && carveTube->weight_ok && ctx.carvePos.IsCarveEnable()) ? true : false;
        write_plc_bool(ctx.gplatConn, "CARVE_ENABLE", carveStatus, &err);
        spdlog::info("发出压印允许信号: {}", carveStatus);
    }
    else
    {
        bool carveStatus = ctx.carvePos.IsCarveEnable() ? true : false;
        write_plc_bool(ctx.gplatConn, "CARVE_ENABLE", carveStatus, &err);
        spdlog::info("发出压印允许信号: {}", carveStatus);
    }
}

//--------处理对齐信号ALIGN_POS_ON--------
void handleAlignPosOn(TubeTrackContext &ctx, const char *value)
{
    bool isOn = read_value<bool>(value);
    spdlog::info("ALIGN_POS_ON isOn: {}", isOn);

    if (isOn)
    {
        // 执行对齐工位有料状态的相关操作
        if (!ctx.alignPos.IsEmpty())
        {
            spdlog::warn("Align position is not empty, cannot move tube from Production plan");
            return;
        }

        auto tube = ctx.prodPlan.Pop();

        if (!tube)
        {
            spdlog::warn("Production plan is empty, no tube to move to align position");
            return;
        }

        ctx.alignPos.Push(std::move(tube));
        ctx.alignPos.DebugOut();

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
    spdlog::info("WEIGHT_POS_ON isOn: {}", isOn);
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

        // 管子进入称重工位后触发称重功能
        // const CTube *tube = ctx.weightPos.Peek();
        // if (tube != nullptr)
        // {
        //     unsigned int error;
        //     int a = 1;
        //     bool ret = writeb(ctx.gplatConn, "START_WEIGHT_EVENT", &a, sizeof(a), &error);
        //     spdlog::info("启动称重");
        // }
        // else
        // {
        //     spdlog::warn("No tube in weight position to start weighing");
        // }
    }
}

void handleWeiPosOnDelay(TubeTrackContext &ctx, const char *value)
{
    bool isOn = read_value<bool>(value);

    if (isOn)
    {
        // 管子进入称重工位后延时触发称重功能
        const CTube *tube = ctx.weightPos.Peek();
        if (tube != nullptr)
        {
            unsigned int error;
            int a = 1;
            bool ret = writeb(ctx.gplatConn, "START_WEIGHT_EVENT", &a, sizeof(a), &error);
            spdlog::info("启动称重");
        }
        else
        {
            spdlog::warn("No tube in weight position to start weighing");
        }
    }
}

//--------处理刻印信号PRT_POS_ON--------
void handlePrtPosOn(TubeTrackContext &ctx, const char *value)
{
    bool isOn = read_value<bool>(value);
    spdlog::info("CARVE_POS_ON isOn: {}", isOn);
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
    spdlog::info("SPRAY_POS_ON isOn: {}", isOn);
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

        // spdlog::info("判断是否要启动测长");
        // const CTube *tube = ctx.sprayPos.Peek();
        // if (tube != nullptr)
        // {
        //     if (tube->length_ok && tube->weight_ok)
        //     {
        //         unsigned int err;
        //         write_plc_bool(ctx.gplatConn, "LENGTH_START", true, &err); // 启动测长
        //         spdlog::info("启动测长");
        //     }
        // }
    }
}

void handleSpyPosOnDelay(TubeTrackContext &ctx, const char *value)
{
    bool isOn = read_value<bool>(value);

    if (isOn)
    {
        spdlog::info("判断是否要启动测长");
        const CTube *tube = ctx.sprayPos.Peek();
        if (tube != nullptr)
        {
            if (tube->length_ok && tube->weight_ok)
            {
                unsigned int err;
                write_plc_bool(ctx.gplatConn, "LENGTH_START", true, &err); // 启动测长
                spdlog::info("启动测长");
            }
        }
    }
}

//--------处理色环信号CIR_POS_ON--------
void handleCirPosOn(TubeTrackContext &ctx, const char *value)
{
    bool isOn = read_value<bool>(value);
    spdlog::info("CIRCLE_POS_ON isOn: {}", isOn);
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
    spdlog::info("SCRAPTROLLER_POS_ON isOn: {}", isOn);
    if (!isOn)
    {
        // 从废料辊道弹出管子，推送到缓冲区
        auto tube = ctx.scraptRoller.Pop();

        if (!tube)
        {
            spdlog::warn("Scrap roller is empty, no tube to move to back buffer");
            return;
        }

        if (tube->weight_ok && tube->length_ok)
        {
            ctx.backBuffer.Push(std::move(tube));
            ctx.backBuffer.DebugOut();
        }
        else
        {
            ctx.scrapt.Push(std::move(tube));
            ctx.scrapt.DebugOut();
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

    if (isOn)
    {
        // 复位发给PLC的命令信号
        unsigned int err;
        write_plc_bool(ctx.gplatConn, "SPRAY_START_NOUSE", false, &err);
    }
}

void autoBundle(TubeTrackContext &ctx)
{
    int buffercount, basketcount, bundlecount;

    buffercount = ctx.backBuffer.Count();
    basketcount = ctx.basket.Count();
    bundlecount = ctx.basket.BundleCount();

    // spdlog::info("自动打捆: 缓冲区管子数={}, 成品料筐管子数={}, 打捆根数={}", buffercount, basketcount, bundlecount);

    // 如果缓冲区的管子数小于5支,或者成品料筐里有管子,或者缓冲区根数小于打捆根数则不启动自动打捆
    if (buffercount < 5 || basketcount > 0 || buffercount < bundlecount)
        return;

    MoveTubeCmd cmd;
    cmd.from = std::string("backbuffer");
    cmd.to = std::string("basket");
    spdlog::info("自动打捆: 从缓冲区移动管子到成品料筐, 打捆根数={}, from={}, to={}", bundlecount, cmd.from.c_str(), cmd.to.c_str());
    for (int i = 0; i < bundlecount; i++)
    {
        executeMoveTubeCmd(ctx, cmd);
    }

    // MoveTubeCmd cmd;
    // cmd.from = "backbuffer";
    // cmd.to = "basket";
    // spdlog::info("自动打捆: 从缓冲区移动管子到成品料筐, 打捆根数={}, from={}, to={}", bundlecount, cmd.from.c_str(), cmd.to.c_str());
    // unsigned int err;
    // for (int i = 0; i < bundlecount; i++)
    // {
    //     writeb(ctx.gplatConn, "MOVE_TUBE_CMD", &cmd, sizeof(cmd), &err);
    // }

    spdlog::info("自动打捆: 从缓冲区移动管子到成品料筐");
}