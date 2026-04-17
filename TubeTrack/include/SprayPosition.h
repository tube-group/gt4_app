// SprayPosition.h 喷印工位类定义
#pragma once
#include "PositionBase.h"

class CSprayPosition : public CPositionBase
{
public:
    CSprayPosition() = default;
    ~CSprayPosition() = default;

    virtual void UpdateForm() override; // 刷新画面

private:
    const char *REDIS_KEY = "SPRAY_POS_TUBE_INFO"; // 发布的key名称
};