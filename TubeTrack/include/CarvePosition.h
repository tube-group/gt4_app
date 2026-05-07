// CarvePosition.h 刻印工位类定义
#pragma once
#include "PositionBase.h"

class CCarvePosition : public CPositionBase
{
public:
    CCarvePosition(string redisKey, string positionName) : CPositionBase(redisKey, positionName) {}

    virtual void UpdateForm() override; // 刷新画面
    void ReadParameterSet(); // 从数据库读取刻印工位参数
private:
    int carve_enable_; // 刻印允许
};