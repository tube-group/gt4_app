// ProductionPlan.cpp 投料计划实现
#include "ProductionPlan.h"
#include "TubeTrackObject.h" // 访问全局变量
#include <nlohmann/json.hpp>

// bool CProductionPlan::Pop(CTube *pTube, int /*mode*/)
// {
//     static int flow_no = 0;
//     /* 填充管子数据 */;
//     (*pTube).calib_tube = false;
//     (*pTube).order_no = "12345678";
//     (*pTube).item_no = "ITEM001";
//     (*pTube).roll_no = "ROLL1234";
//     (*pTube).melt_no = "MELT5678";
//     (*pTube).lot_no = "LOT91011";
//     (*pTube).tube_no = 1;
//     (*pTube).flow_no = flow_no++;
//     (*pTube).lotno_coupling = "COUPLELOT";
//     (*pTube).meltno_coupling = "COUPLEMELT";
//     (*pTube).length = 6.0f;
//     (*pTube).weight = 12.5f;
//     (*pTube).lengthOk = true;
//     (*pTube).weightOk = true;
//     (*pTube).bSprayed = false;

//     // 投料计划的出栈逻辑实现
//     return true; // 示例返回值
// }

bool CProductionPlan::Pop(CTube *pTube, int /*mode*/)
{

    if (feed_num > 0)
    {
        // 查询当前合同和生产参数，填充管子数据
        pTube->order_no = order_no;
        pTube->roll_no = roll_no;
        pTube->item_no = item_no;
        pTube->lot_no = lot_no;
        pTube->melt_no = melt_no;
        pTube->meltno_coupling = meltno_coupling;
        pTube->lotno_coupling = lotno_coupling;
        pTube->tube_no = tube_no;

        // 生成流水号
        static int flow_no = 0;
        pTube->flow_no = flow_no++;

        // 更新计数器
        feed_num--;
        tube_no += 1;

        return true;
    }
    else
    {
        return false;
    }
}

string CProductionPlan::convertToJson(const CProductionPlan &plan)
{
    // 使用nlohmann/json库实现生产计划转换为JSON格式字符串
    nlohmann::json j;
    j["order_no"] = plan.order_no;
    j["item_no"] = plan.item_no;
    j["roll_no"] = plan.roll_no;
    j["melt_no"] = plan.melt_no;
    j["lot_no"] = plan.lot_no;
    j["lotno_coupling"] = plan.lotno_coupling;
    j["meltno_coupling"] = plan.meltno_coupling;
    j["feed_num"] = plan.feed_num;
    j["tube_no"] = plan.tube_no;

    return j.dump(4);
}

// 初始化静态常量
// const char *CProductionPlan::REDIS_KEY = "PlanInfo";

void CProductionPlan::UpdateForm()
{
    // 检查全局Redis连接
    if (g_redis == nullptr)
    {
        std::cerr << "错误：Redis连接未初始化" << std::endl;
        return;
    }

    try
    {
        // 转化为json格式输出
        string jsonStr = convertToJson(*this);

        // 检查JSON是否有效
        if (jsonStr.empty())
        {
            std::cerr << "错误：生成的JSON字符串为空！" << std::endl;
            return;
        }

        std::cout << "生成的JSON: " << jsonStr << std::endl;

        // 写入Redis数据库
        g_redis->set(REDIS_KEY, jsonStr);

        // 发布详细消息到 RealDataChanged 主题
        g_redis->publish("RealDataChanged", REDIS_KEY);

        std::cout << "✓ 已更新Redis并发布通知:" << REDIS_KEY << std::endl;
    }
    catch (const std::exception &e)
    {
        std::cerr << "UpdateForm异常: " << e.what() << std::endl;
    }
}