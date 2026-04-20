// Scrapt.h 废料筐 类定义
#pragma once
#include "PositionBase.h"

class CScrapt : public CPositionBase
{
    public:
    CScrapt() = default;
    ~CScrapt() = default;

    virtual void UpdateForm() override; // 刷新画面
    private:
    const char *REDIS_KEY = "SCRAPT_POS_TUBE_INFO"; // 发布的key名称
};