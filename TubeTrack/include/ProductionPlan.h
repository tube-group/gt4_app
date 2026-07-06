// ProductionPlan.h 投料计划 类定义
#pragma once
#include "Tube.h"
#include <memory>

struct TubeTrackContext; // 前向声明

class CProductionPlan
{
private:
    TubeTrackContext* m_ctx = nullptr; // 上下文指针
    string m_redisKey; // Redis键名称
	string m_positionName; // 工位名称（用于日志）
    string convertToJson();

public:
    CProductionPlan(string redisKey, string positionName) : m_redisKey(redisKey), m_positionName(positionName) {}
    void SetContext(TubeTrackContext& ctx) { m_ctx = &ctx; }
    std::unique_ptr<CTube> Pop(int mode = 0);
    bool Push(std::unique_ptr<CTube> tube, int mode = 0);
    void UpdateForm();
    bool ApplyCurrentContract(const string &orderNo, const string &itemNo);
    void RestoreFromRedis();
    void RestoreFromJson(const string &jsonStr);
    void ReadParameterSet();
    bool IsEmpty();
    int Count(){return feed_num_;} // 返回投料支数

private:
    string order_no_;        // 合同号
    string item_no_;         // 项目号
    string roll_no_;         // 轧批号
    string melt_no_;         // 炉号
    string lot_no_;          // 试批号
    string lotno_coupling_;  // 接箍批号
    string meltno_coupling_; // 接箍炉号
    int feed_num_ = 0;       // 投料支数
    int tube_no_ = 0;        // 管号
    int flow_no_ = 0;        // 流水号
};