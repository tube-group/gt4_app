// WeightPosition.h 称重工位类定义
#pragma once
#include "PositionBase.h"

class CWeightPosition : public CPositionBase
{
    public:
        CWeightPosition() = default;
        ~CWeightPosition() = default;

        virtual void UpdateForm() override; // 刷新画面
};