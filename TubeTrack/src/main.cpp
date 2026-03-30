#include <iostream>
#include "TubeTrackObject.h"
#include <sw/redis++/redis++.h>
#include <unistd.h>  // 用于sleep函数

int main()
{
    // 1. 连接 Redis
    std::cout << "正在连接 Redis..." << std::endl;
    try {
        sw::redis::ConnectionOptions opts;
        opts.host = "140.32.1.192";
        opts.port = 6379;
        opts.password = "ggl2e=mc2";
        
        // 创建Redis连接并赋值给全局变量
        g_redis = std::make_unique<sw::redis::Redis>(opts);
        
        // 测试连接
        g_redis->ping();
        std::cout << "成功连接到 Redis" << std::endl;
        
    } catch (const std::exception& e) {
        std::cerr << "Redis连接失败: " << e.what() << std::endl;
        return -1;
    }

    // 2. 初始化生产计划数据
    prodPlan.order_no = "20240001";
    prodPlan.item_no = "ITEM001";
    prodPlan.roll_no = "ROLL1234";
    prodPlan.melt_no = "MELT5678";
    prodPlan.lot_no = "LOT91011";
    prodPlan.lotno_coupling = "COUPLELOT";
    prodPlan.meltno_coupling = "COUPLEMELT";
    prodPlan.feed_num = 100;  // 初始投料支数
    prodPlan.tube_no = 0;     // 初始管号

    // 3. 初始同步到Redis
    prodPlan.UpdateForm();

    // 4. 模拟生产流程
    CTube tube;
    for (int i = 0; i < 100; i++)
    {
        // 从投料计划中获取管子数据
        if (prodPlan.Pop(&tube))
        {
            // 将管子数据推送到测长工位
            if (lengthPos.Push(tube))
            {
                // 成功推送后，输出工位状态
                lengthPos.DebugOut();

                // 从测长工位弹出管子数据
                lengthPos.Pop(&tube);
            }

            // 更新Redis数据
            prodPlan.UpdateForm();
        }

        sleep(1); // 模拟生产节奏
    }

    return 0;
}