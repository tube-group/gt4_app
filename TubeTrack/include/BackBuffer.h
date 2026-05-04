// BackBuffer.h 测量点后缓冲区 类定义
#pragma once
#include "PositionBase.h"

class CBackBuffer : public CPositionBase
{   
    public:
        CBackBuffer(string redisKey, string positionName) : CPositionBase(redisKey, positionName) {}

        virtual void UpdateForm() override; // 刷新画面
        virtual void DebugOut() override;   // 输出缓冲区管子数量
};