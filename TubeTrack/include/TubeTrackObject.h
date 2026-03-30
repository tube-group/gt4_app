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
#include <sw/redis++/redis++.h>  // Redis++ 库
#include <memory>  // 用于智能指针

// 声明工位全局变量
extern CProductionPlan prodPlan;
extern CLengthPosition lengthPos;
extern CCarvePosition carvePos;
extern CWeightPosition weightPos;
extern CSprayPosition sprayPos;
extern CCirclePosition circlePos;
extern CScraptRoller scraptRoller;
extern CBackBuffer backBuffer;
extern CScrapt scrapt;
extern CBasket basket;

// 声明全局Redis连接
extern std::unique_ptr<sw::redis::Redis> g_redis;