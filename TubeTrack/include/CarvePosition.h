// CarvePosition.h 刻印工位类定义
#pragma once
#include "PositionBase.h"

class CCarvePosition : public CPositionBase
{
public:
    CCarvePosition() = default;
    ~CCarvePosition() = default;

    virtual void UpdateForm() override; // 刷新画面

private:
    const char *REDIS_KEY = "CARVE_POS_TUBE_INFO"; // 发布的key名称
};