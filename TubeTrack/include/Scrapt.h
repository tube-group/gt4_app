// Scrapt.h 废料筐 类定义
#pragma once
#include "PositionBase.h"

class CScrapt : public CPositionBase
{
public:
    CScrapt(string redisKey, string positionName) : CPositionBase(redisKey, positionName) {}

    void UpdateForm() override; // 刷新画面
    void DebugOut() override;   // 输出废料筐管子数量
};