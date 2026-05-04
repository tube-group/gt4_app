// SprayPosition.h 喷印工位类定义
#pragma once
#include "PositionBase.h"

class CSprayPosition : public CPositionBase
{
public:
    CSprayPosition(string redisKey, string positionName) : CPositionBase(redisKey, positionName) {}

    virtual void UpdateForm() override; // 刷新画面
};