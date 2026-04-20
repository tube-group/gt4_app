// Basket.h 成品料筐 类定义
#pragma once
#include "PositionBase.h"

class CBasket : public CPositionBase
{
public:
    CBasket() = default;
    ~CBasket() = default;

    virtual void UpdateForm() override; // 刷新画面
    virtual void DebugOut() override;   // 输出缓冲区管子数量

private:
    const char *REDIS_KEY = "BASKET_TUBE_INFO"; // 发布的key名称
};