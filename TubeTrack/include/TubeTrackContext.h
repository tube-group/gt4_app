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
#include <memory>

struct TubeTrackContext {
    // 共享资源
    std::unique_ptr<sw::redis::Redis> redis;

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
};
