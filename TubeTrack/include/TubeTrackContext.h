// TubeTrackContext.h
// 将所有工位对象和共享资源收敛到一个上下文结构体中，
// 在 main() 中创建，通过引用传递给需要的模块。
#pragma once

#include "ProductionPlan.h"
#include "LengthPosition.h"
#include "CarvePosition.h"
#include "WeightPosition.h"
#include "SprayPosition.h"
#include "CirclePosition.h"
#include "ScraptRoller.h"
#include "BackBuffer.h"
#include "Scrapt.h"
#include "Basket.h"
#include <sw/redis++/redis++.h>
#include "../include/higplat.h"
#include <memory>

struct TubeTrackContext {
    // 共享资源
    std::unique_ptr<sw::redis::Redis> redis;
    int gplatConn = -1;

    // 工位对象
    CProductionPlan prodPlan;
    CLengthPosition lengthPos;
    CCarvePosition  carvePos;
    CWeightPosition weightPos;
    CSprayPosition  sprayPos;
    CCirclePosition circlePos;
    CScraptRoller   scraptRoller;
    CBackBuffer     backBuffer;
    CScrapt         scrapt;
    CBasket         basket;

    // 统一注入上下文到所有工位
    void Init() {
        prodPlan.SetContext(*this);
        lengthPos.SetContext(*this);
        carvePos.SetContext(*this);
        weightPos.SetContext(*this);
        sprayPos.SetContext(*this);
        circlePos.SetContext(*this);
        scraptRoller.SetContext(*this);
        backBuffer.SetContext(*this);
        scrapt.SetContext(*this);
        basket.SetContext(*this);
    }

    // 清理资源
    void Cleanup() {
        if (gplatConn > 0) {
            disconnectgplat(gplatConn);
            gplatConn = -1;
        }
        redis.reset();
    }
};
