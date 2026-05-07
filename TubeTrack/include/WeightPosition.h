// WeightPosition.h 称重工位类定义
#pragma once
#include "PositionBase.h"

class CWeightPosition : public CPositionBase
{
public:
    CWeightPosition(string redisKey, string positionName) : CPositionBase(redisKey, positionName) {}

    virtual void UpdateForm() override; // 刷新画面
    void ReadParameterSet();            // 从数据库读取称重工位参数

private:
    int weight_enable_;       // 称重允许
    int waste_weight_enable_; // 重量判废
    float weight_limit_max_;   // 管重偏差上限
    float weight_limit_min_;   // 管重偏差下限
    float weight_per_meter_;   // 米重(kg/m)
    float weight_ew_;          // EW值
    int weight_coupling_ = 0; // 保护环重量(0.01KG)
};