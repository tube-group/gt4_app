// Basket.h 成品料筐 类定义
#pragma once
#include "PositionBase.h"

class CBasket : public CPositionBase
{
public:
    CBasket(string redisKey, string positionName) : CPositionBase(redisKey, positionName) {}

    void UpdateForm() override;                                // 刷新画面
    void DebugOut() override;                                  // 输出成品料筐管子数量
    void ReadParameterSet();                                   // 从数据库读取成品料筐参数
    int BundleCount();                                         // 计算当前成品料筐内管子数量是否满足打捆要求
    bool HasSpace() ;                                  // 判断成品料筐是否有空间放置新管子
    void EntryTrigger(const CTube &tube) override;     // 打捆触发函数
    bool PushFront(unique_ptr<CTube> tube, int mode) override; // 重写PushFront方法，添加打捆逻辑
    bool Bundle();                                             // 执行打捆操作，成功后生成管捆信息并清空成品料筐

private:
    int bundle_number_;     // 打捆根数
    int bundle_flow_no_;    // 管捆流水号
    int bundle_first_type_; // 管捆号首位(1油管2套管）

    int max_count_ = 100; // 成品料筐最大容量，默认值为100根
    string bundle_type;          // 管捆类型
    double weight_packaging;    // 包装材料重量
};