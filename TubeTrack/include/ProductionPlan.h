// ProductionPlan.h 投料计划 类定义
#pragma once
#include "Tube.h"
#include <memory>

struct TubeTrackContext; // 前向声明

class CProductionPlan
{
private:
    TubeTrackContext* m_ctx = nullptr; // 上下文指针
    string convertToJson();

public:
    void SetContext(TubeTrackContext& ctx) { m_ctx = &ctx; }
    std::unique_ptr<CTube> Pop(int mode = 0);
    void UpdateForm();
    void Initialize();
    bool ApplyCurrentContract(const string &orderNo, const string &itemNo);
    bool RestoreFromJson(const string &jsonStr);
    bool IsEmpty();

private:
    string order_no;        // 合同号
    string item_no;         // 项目号
    string roll_no;         // 轧批号
    string melt_no;         // 炉号
    string lot_no;          // 试批号
    string lotno_coupling;  // 接箍批号
    string meltno_coupling; // 接箍炉号
    int feed_num;           // 投料支数
    int tube_no;            // 管号

    const char *REDIS_KEY = "PlanInfo"; // 发布的key名称
};