// WeightPosition.h 称重工位类定义
#pragma once
#include "PositionBase.h"

class CWeightPosition : public CPositionBase
{
public:
    CWeightPosition(string redisKey, string positionName) : CPositionBase(redisKey, positionName) {}

    virtual void UpdateForm() override; // 刷新画面
};