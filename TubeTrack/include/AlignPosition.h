// AlignPosition.h 对齐工位类定义
#pragma once
#include "PositionBase.h"

class CAlignPosition : public CPositionBase
{
public:
    CAlignPosition(string redisKey, string positionName) : CPositionBase(redisKey, positionName) {}

    virtual void UpdateForm() override; // 刷新画面
};