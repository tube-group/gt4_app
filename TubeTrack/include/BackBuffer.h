// BackBuffer.h 测量点后缓冲区 类定义
#pragma once
#include "PositionBase.h"

class CBackBuffer : public CPositionBase
{   
    public:
        CBackBuffer(string redisKey, string positionName) : CPositionBase(redisKey, positionName) {}

        void UpdateForm() override; // 刷新画面
        void DebugOut() override;   // 输出缓冲区管子数量
        bool QueryTube(const std::string& order_no, int flow_no) const;// 根据订单号和流水号查询管子是否在后缓冲区中
};