// SprayPosition.h 喷印工位类定义
#pragma once
#include "PositionBase.h"

class CSprayPosition : public CPositionBase
{
public:
    CSprayPosition(string redisKey, string positionName) : CPositionBase(redisKey, positionName) {}

    virtual void UpdateForm() override; // 刷新画面
    void ReadParameterSet();            // 从数据库读取喷印工位参数
private:
    int length_enable_;          // 测长允许
    int waste_length_enable_;    // 长度判废
    float length_limit_max_;     // 判废管长止
    float length_limit_min_;     // 判废管长起
    float length_coupling_ = 0;  // 保护环长度
    int spray_length_precision_; // 喷涂长度小数位数
    // bool m_bManualLength;

    int waste_weight_enable_; // 重量判废
    float weight_limit_max_;  // 管重偏差上限
    float weight_limit_min_;  // 管重偏差下限
    float weight_per_meter_;  // 米重(kg/m)
    float weight_ew_;         // EW值

    int spray_length_type_;  // 喷涂长度格式（0公制1英制）
    int spray_weight_type_;  // 喷涂重量格式（0公制1英制）
    int spray_enable_; // 喷印允许
    int qrcode_spray_enable_;//二维码喷印

    int flow_no_; // 喷印工位下一根管子流水号
};