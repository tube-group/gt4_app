// ProductionPlan.h 投料计划 类定义
#pragma once
#include "Tube.h"

class CProductionPlan
{
private:
    string convertToJson(const CProductionPlan &plan);

public:
    bool Pop(CTube *pTube, int mode = 0);
    void UpdateForm();

public:
    string order_no;        // 合同号
    string item_no;         // 项目号
    string roll_no;         // 轧批号
    string melt_no;         // 炉号
    string lot_no;          // 试批号
    string lotno_coupling;  // 接箍批号
    string meltno_coupling; // 接箍炉号
    int feed_num;           // 投料支数
    int tube_no;            // 管号

private:
    const char *REDIS_KEY = "PlanInfo"; // 发布的key名称
};