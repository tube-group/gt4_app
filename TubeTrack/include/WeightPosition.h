// WeightPosition.h 称重工位类定义
#pragma once
#include "PositionBase.h"

class CWeightPosition : public CPositionBase
{
public:
    CWeightPosition(string redisKey, string positionName) : CPositionBase(redisKey, positionName) {}

    void UpdateForm() override;                    // 刷新画面
    void ReadParameterSet();                       // 从数据库读取称重工位参数
    void SetTubeWeight(int weight);                // 获取管子实际重量，并设置到管子对象中
    void EntryTriggerBeforePush(CTube &tube) override; // 在Push管子进入称重工位前触发
    void EntryTrigger(const CTube &tube) override; // 重写触发器，进行称重计算和判定

private:
    int weight_enable_;       // 称重允许
    int waste_weight_enable_; // 重量判废
    float weight_limit_max_;  // 管重偏差上限
    float weight_limit_min_;  // 管重偏差下限
    float weight_per_meter_;  // 米重(kg/m)
    float weight_ew_;         // EW值
    float weight_coupling_;   // 保护环重量(0.01KG)
};