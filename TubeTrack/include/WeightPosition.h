// WeightPosition.h 称重工位类定义
#pragma once
#include "PositionBase.h"

class CWeightPosition : public CPositionBase
{
public:
    CWeightPosition() = default;
    ~CWeightPosition() = default;

    virtual void UpdateForm() override; // 刷新画面
private:
    const char *REDIS_KEY = "WEIGHT_POS_TUBE_INFO"; // 发布的key名称
};