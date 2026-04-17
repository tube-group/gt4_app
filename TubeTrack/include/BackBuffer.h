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

private:
    const char *REDIS_KEY = "BACK_BUFFER_TUBE_INFO"; // 发布的key名称
};