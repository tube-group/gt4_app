// ScraptRoller.h 废料辊道 类定义
#pragma once
#include "PositionBase.h"

class CScraptRoller : public CPositionBase
{
public:
    CScraptRoller(string redisKey, string positionName) : CPositionBase(redisKey, positionName) {}

    virtual void UpdateForm() override; // 刷新画面
};