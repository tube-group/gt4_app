// ScraptRoller.h 废料辊道 类定义
#pragma once
#include "PositionBase.h"

class CScraptRoller : public CPositionBase
{
    public:
    CScraptRoller() = default;
    ~CScraptRoller() = default;

    virtual void UpdateForm() override; // 刷新画面

    private:
    const char *REDIS_KEY = "SCR_ROLLER_POS_TUBE_INFO"; //
};