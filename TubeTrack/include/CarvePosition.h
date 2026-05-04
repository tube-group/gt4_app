// CarvePosition.h 刻印工位类定义
#pragma once
#include "PositionBase.h"

class CCarvePosition : public CPositionBase
{
public:
    CCarvePosition(string redisKey, string positionName) : CPositionBase(redisKey, positionName) {}

    virtual void UpdateForm() override; // 刷新画面
};