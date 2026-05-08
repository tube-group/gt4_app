// CarvePosition.h 刻印工位类定义
#pragma once
#include "PositionBase.h"
#include <array>

class CCarvePosition : public CPositionBase
{
public:
    CCarvePosition(string redisKey, string positionName) : CPositionBase(redisKey, positionName) { carve_enable_ = 0; }; // 构造函数

    virtual void UpdateForm() override; // 刷新画面
    void ReadParameterSet(); // 从数据库读取刻印工位参数
    bool IsCarveEnabled() const { return carve_enable_; } // 判断刻印是否允许
    bool PushBack(unique_ptr<CTube> tube, int mode = 0) override; // 重写PushBack方法，增加刻印工位特有的逻辑
    void HandleCarveFinish(int status); // 处理刻印完成后的逻辑

private:
    using CarveReqArray = std::array<std::string, 8>;// 刻印参数数组类型定义，参数个数是8个

    void ReplaceAll(std::string &text, const std::string &from, const std::string &to) const;// 字符串替换工具函数
    void ConvertCarveRequests(CarveReqArray &requests) const;// 刻印参数格式转换
    bool LoadCarveRequests(const CTube &tube, CarveReqArray &requests) const;// 从数据库加载刻印参数
    bool SendCarveRequestsToPlc(const CarveReqArray &requests) const;// 将刻印参数发送给PLC

    int carve_enable_; // 刻印允许
};