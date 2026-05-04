// Basket.h 成品料筐 类定义
#pragma once
#include "PositionBase.h"

class CBasket : public CPositionBase
{
public:
    CBasket(string redisKey, string positionName) : CPositionBase(redisKey, positionName) {}

    virtual void UpdateForm() override; // 刷新画面
    virtual void DebugOut() override;   // 输出成品料筐管子数量
};