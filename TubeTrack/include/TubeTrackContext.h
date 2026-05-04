// TubeTrackContext.h
// 将所有工位对象和共享资源收敛到一个上下文结构体中，
// 在 main() 中创建，通过引用传递给需要的模块。
#pragma once

#include "ProductionPlan.h"
#include "AlignPosition.h"
#include "CarvePosition.h"
#include "WeightPosition.h"
#include "SprayPosition.h"
#include "CirclePosition.h"
#include "ScraptRoller.h"
#include "BackBuffer.h"
#include "Scrapt.h"
#include "Basket.h"
#include "WalkingBeam.h"
#include <sw/redis++/redis++.h>
#include <pqxx/pqxx>
#include "logging.h"   // spdlog
#include "higplat.h"
#include <memory>

struct TubeTrackContext {
    // 共享资源
    std::unique_ptr<sw::redis::Redis> redis;
    int gplatConn = -1;
    std::unique_ptr<pqxx::connection> pgConn;

    // 工位对象
    CProductionPlan prodPlan{"PlanInfo", "生产计划工位"}; // 生产计划工位
    CAlignPosition alignPos{"ALIGN_POS_TUBE_INFO", "对齐工位"}; // 对齐工位
    CCarvePosition  carvePos{"CARVE_POS_TUBE_INFO", "刻印工位"}; // 刻印工位
    CWeightPosition weightPos{"WEIGHT_POS_TUBE_INFO", "称重工位"}; // 称重工位
    CSprayPosition  sprayPos{"SPRAY_POS_TUBE_INFO", "喷印工位"}; // 喷印工位
    CCirclePosition circlePos{"CIRCLE_POS_TUBE_INFO", "色环工位"}; // 色环工位
    CScraptRoller   scraptRoller{"SCRAPTROLLER_POS_TUBE_INFO", "废料辊道工位"}; // 废料辊道工位
    CBackBuffer     backBuffer{"BACKBUFFER_POS_TUBE_INFO", "缓冲区工位"}; // 缓冲区工位
    CScrapt         scrapt{"SCRAPT_POS_TUBE_INFO", "废料筐工位"}; // 废料筐工位
    CBasket         basket{"BASKET_POS_TUBE_INFO", "成品筐工位"}; // 成品筐工位
    WalkingBeam     walkingBeam; // 步进梁工位

    // 统一注入上下文到所有工位
    void Init() {
        prodPlan.SetContext(*this);
        alignPos.SetContext(*this);
        carvePos.SetContext(*this);
        weightPos.SetContext(*this);
        sprayPos.SetContext(*this);
        circlePos.SetContext(*this);
        scraptRoller.SetContext(*this);
        backBuffer.SetContext(*this);
        scrapt.SetContext(*this);
        basket.SetContext(*this);
        walkingBeam.SetContext(*this);

        // 从Redis恢复工位状态
        // 1. 定义Lambda表达式（复用恢复逻辑）
        // auto restorePosition = [this](CPositionBase &position, const char *redisKey, const char *positionName)
        // {
        //     if (!redis)
        //     {
        //         return false;
        //     }
        //     // 从Redis获取数据并恢复
        //     auto value = redis->get(redisKey);
        //     if (!value)
        //     {
        //         spdlog::info("Redis中未找到{}数据，初始化为空工位", positionName);
        //         position.Clear();  // 确保为空
        //         return true;  // 返回true表示正常状态
        //     }
        //     // 调用工位的恢复方法
        //     return position.RestoreFromJson(*value, positionName);
        // };

        // 2. 恢复生产计划
        prodPlan.RestoreFromRedis(); // 先尝试从Redis恢复生产计划
        // bool planRestored = false;
        // if (redis)
        // {
        //     // 从Redis获取生产计划数据并恢复
        //     auto planValue = redis->get("PlanInfo");
        //     if (planValue)
        //     {
        //         planRestored = prodPlan.RestoreFromJson(*planValue);
        //     }
        // }

        // if (!planRestored)
        // {
        //     prodPlan.Initialize(); // 无Redis状态时回退到数据库初始化生产计划
        // }

        // 3. 恢复各个工位状态
        alignPos.RestoreFromRedis();
        weightPos.RestoreFromRedis();   
        carvePos.RestoreFromRedis();
        sprayPos.RestoreFromRedis();
        circlePos.RestoreFromRedis();
        scraptRoller.RestoreFromRedis();
        backBuffer.RestoreFromRedis();
        scrapt.RestoreFromRedis();
        basket.RestoreFromRedis();
        // restorePosition(alignPos, "ALIGN_POS_TUBE_INFO", "对齐工位");
        // restorePosition(weightPos, "WEIGHT_POS_TUBE_INFO", "称重工位");
        // restorePosition(carvePos, "CARVE_POS_TUBE_INFO", "刻印工位");
        // restorePosition(sprayPos, "SPRAY_POS_TUBE_INFO", "喷印工位");
        // restorePosition(circlePos, "CIRCLE_POS_TUBE_INFO", "色环工位");
        // restorePosition(scraptRoller, "SCRAPTROLLER_POS_TUBE_INFO", "废料辊道");
        // restorePosition(backBuffer, "BACKBUFFER_POS_TUBE_INFO", "缓冲区");
        // restorePosition(scrapt, "SCRAPT_POS_TUBE_INFO", "废料筐");
        // restorePosition(basket, "BASKET_POS_TUBE_INFO", "成品筐");
    }

    // 清理资源
    void Cleanup() {
        if (gplatConn > 0) {
            disconnectgplat(gplatConn);
            gplatConn = -1;
        }
        redis.reset();
        pgConn.reset();
    }
};
