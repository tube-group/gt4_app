// TubeTrackObject.cpp
// 在此文件中以「全局变量」形式创建所有工位的实例

#include "TubeTrackObject.h"
#include <memory>

// 全局实例（程序启动前构造，程序退出时自动析构）
CProductionPlan prodPlan;
CLengthPosition lengthPos;
CCarvePosition carvePos;
CWeightPosition weightPos;
CSprayPosition sprayPos;
CCirclePosition circlePos;
CScraptRoller scraptRoller;
CBackBuffer backBuffer;
CScrapt scrapt;
CBasket basket;

// 定义全局Redis连接
std::unique_ptr<sw::redis::Redis> g_redis = nullptr;
