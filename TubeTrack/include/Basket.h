// Basket.h 成品料筐 类定义
#pragma once
#include "PositionBase.h"

class CBasket : public CPositionBase
{
public:
    CBasket(string redisKey, string positionName) : CPositionBase(redisKey, positionName) {}

    virtual void UpdateForm() override; // 刷新画面
    virtual void DebugOut() override;   // 输出成品料筐管子数量
    void ReadParameterSet();            // 从数据库读取成品料筐参数

private:
    int bundle_number_;     // 打捆根数
    int bundle_flow_no_;    // 管捆流水号
    int bundle_first_type_; // 管捆号首位(1油管2套管）
};