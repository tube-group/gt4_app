// CirclePosition.h 色环工位类定义
#pragma once
#include "PositionBase.h"

class CCirclePosition : public CPositionBase
{
public:
    CCirclePosition(string redisKey, string positionName) : CPositionBase(redisKey, positionName) {}

    virtual void UpdateForm() override; // 刷新画面
};