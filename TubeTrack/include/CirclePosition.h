// CirclePosition.h 色环工位类定义
#pragma once
#include "PositionBase.h"

class CCirclePosition : public CPositionBase
{
public:
    CCirclePosition(string redisKey, string positionName) : CPositionBase(redisKey, positionName) {}

    void EntryTrigger(const CTube &tube) override; // 当管子进入色环工位时触发
    void UpdateForm() override; // 刷新画面
    void ReadParameterSet();            // 从数据库读取色环工位参数
    bool IsCircleEnable() { return circle_enable_; } // 色环允许
private:
    int circle_enable_ = 1; // 色环允许
};