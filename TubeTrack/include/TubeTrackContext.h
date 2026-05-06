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
        prodPlan.RestoreFromRedis(); 
        alignPos.RestoreFromRedis();
        weightPos.RestoreFromRedis();   
        carvePos.RestoreFromRedis();
        sprayPos.RestoreFromRedis();
        circlePos.RestoreFromRedis();
        scraptRoller.RestoreFromRedis();
        backBuffer.RestoreFromRedis();
        scrapt.RestoreFromRedis();
        basket.RestoreFromRedis();

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
