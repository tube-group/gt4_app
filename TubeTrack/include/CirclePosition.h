// CirclePosition.h 色环工位类定义
#pragma once
#include "PositionBase.h"

class CCirclePosition : public CPositionBase
{
public:
    CCirclePosition() = default;
    ~CCirclePosition() = default;

    virtual void UpdateForm() override; // 刷新画面

private:
    const char *REDIS_KEY = "CIRCLE_POS_TUBE_INFO"; // 发布的key名称
};