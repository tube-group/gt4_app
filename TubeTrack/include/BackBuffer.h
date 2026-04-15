// BackBuffer.h 测量点后缓冲区 类定义
#pragma once
#include "PositionBase.h"

class CBackBuffer : public CPositionBase
{   
    public:
        CBackBuffer() = default;
        ~CBackBuffer() = default;

        virtual void UpdateForm() override; // 刷新画面
        virtual void DebugOut() override;   // 输出缓冲区管子数量

};