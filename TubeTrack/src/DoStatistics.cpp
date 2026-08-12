#include "DoStatistics.h"
#include "user_types.h"

void DoStatistics(TubeTrackContext &ctx)
{
    std::string order_no = ""; // 当前合同号
    std::string item_no = "";  // 当前项目号
    std::string melt_no = "";  // 当前炉号
    std::string lot_no = "";   // 当前试批号
    int order_weight = 0;      // 参数表里的合同重量

    float totallength;
    float totalweight; // 吨
    int totalcount;

    try
    {

        // 查询parameter_set表里order_no、item_no、melt_no、lot_no并赋值给相应变量
        pqxx::work txn(*ctx.pgConn);
        const pqxx::result result = txn.exec("SELECT order_no, item_no, melt_no, lot_no, order_weight FROM parameter_set LIMIT 1");
        if (result.empty())
        {
            spdlog::error("parameter_set表无数据，使用默认参数");
            return;
        }
        else
        {
            const auto &row = result[0];
            order_no = row["order_no"].as<string>();
            item_no = row["item_no"].as<string>();
            melt_no = row["melt_no"].as<string>();
            lot_no = row["lot_no"].as<string>();
            order_weight = row["order_weight"].as<int>(0);
        }

        // 创建统计对象并初始化统计对象
        YieldStatistics yieldStatis{};
        yieldStatis.order_no = order_no;
        yieldStatis.item_no = item_no;
        yieldStatis.melt_no = melt_no;
        yieldStatis.lot_no = lot_no;
        yieldStatis.order_weight = 0.0f;
        yieldStatis.order_length = 0.0f;
        yieldStatis.order_count = 0;
        yieldStatis.lot_weight = 0.0f;
        yieldStatis.lot_length = 0.0f;
        yieldStatis.lot_count = 0;
        yieldStatis.shift_weight = 0.0f;
        yieldStatis.shift_length = 0.0f;
        yieldStatis.shift_count = 0;

        // 1. 查询合同完成量（从API_TUBE_DATA_T）
        // 连接pg数据库根据合同号和项目号查询weight、length生产完成量，并将结果赋值给yieldStatis对象
        const pqxx::result result2 = txn.exec(
            "SELECT COALESCE(sum(WEIGHT),0), COALESCE(sum(LENGTH),0), count(*) "
            "FROM API_TUBE_DATA_T "
            "WHERE order_no = $1 AND item_no = $2",
            pqxx::params{order_no, item_no});
        if (!result2.empty())
        {
            const auto &row = result2[0];

            totalweight = row[0].as<float>(0.0f);
            totallength = row[1].as<float>(0.0f);
            totalcount = row[2].as<int>(0);

            yieldStatis.order_weight = totalweight / 1000.0f; // 转换为吨
            yieldStatis.order_length = totallength;
            yieldStatis.order_count = totalcount;
        }

        // 汇总当前合同完成量 ————各缓存位置累加
        ctx.basket.Statistics(order_no, item_no, totalweight, totallength, totalcount);
        yieldStatis.order_weight += totalweight / 1000.0f;
        yieldStatis.order_length += totallength;
        yieldStatis.order_count += totalcount;

        ctx.backBuffer.Statistics(order_no, item_no, totalweight, totallength, totalcount);
        yieldStatis.order_weight += totalweight / 1000.0f;
        yieldStatis.order_length += totallength;
        yieldStatis.order_count += totalcount;

        ctx.scraptRoller.Statistics(order_no, item_no, totalweight, totallength, totalcount);
        yieldStatis.order_weight += totalweight / 1000.0f;
        yieldStatis.order_length += totallength;
        yieldStatis.order_count += totalcount;

        ctx.circlePos.Statistics(order_no, item_no, totalweight, totallength, totalcount);
        yieldStatis.order_weight += totalweight / 1000.0f;
        yieldStatis.order_length += totallength;
        yieldStatis.order_count += totalcount;

        // 合同重量预警
        float diff = order_weight - yieldStatis.order_weight * 1000.0f;
        if (diff < 2000.0f && diff > 500.0f)
        {
            // walkingBeam.Block();        //封锁步进梁
            std::string temp = "此合同还差" + std::to_string(static_cast<int>(diff)) + "公斤未完成，请注意不要超合同量！";
            // ctx.qbdConn.LogAlarm("yjg4_Alarm", temp, 9);
        }
        if (diff > 0.0f && diff < 500.0f)
        {
            // ctx.walkingBeam.Block(); // 封锁步进梁
            std::string temp = "此合同还差" + std::to_string(static_cast<int>(diff)) + "公斤未完成，请注意不要超合同量！";
            // ctx.qbdConn.LogAlarm("yjg4_Alarm", temp, 9);
        }

        // 2. 查询炉批完成量（从API_BUNDLE_DATA_T）
        // 连接pg数据库根据melt_no和lot_no查询米制重量、米制长度、根数完成量，并将结果赋值给yieldStatis对象
        const pqxx::result result3 = txn.exec(
            "SELECT COALESCE(sum(weight),0), COALESCE(sum(total_length),0), COALESCE(sum(tube),0) "
            "FROM API_BUNDLE_DATA_T "
            "WHERE order_no = $1 AND item_no = $2 AND melt_no = $3 AND lot_no = $4",
            pqxx::params{order_no, item_no, melt_no, lot_no});
        if (!result3.empty())
        {
            const auto &row = result3[0];

            totalweight = row[0].as<float>(0.0f);
            totallength = row[1].as<float>(0.0f);
            totalcount = row[2].as<int>(0);

            yieldStatis.lot_weight = totalweight / 1000.0f; // 转换为吨
            yieldStatis.lot_length = totallength;
            yieldStatis.lot_count = totalcount;
        }
        // 汇总当前炉批完成量 ————各缓存位置累加
        ctx.basket.Statistics(order_no, item_no, melt_no, lot_no, totalweight, totallength, totalcount);
        yieldStatis.lot_weight += totalweight / 1000.0f;
        yieldStatis.lot_length += totallength;
        yieldStatis.lot_count += totalcount;

        ctx.backBuffer.Statistics(order_no, item_no, melt_no, lot_no, totalweight, totallength, totalcount);
        yieldStatis.lot_weight += totalweight / 1000.0f;
        yieldStatis.lot_length += totallength;
        yieldStatis.lot_count += totalcount;

        ctx.scraptRoller.Statistics(order_no, item_no, melt_no, lot_no, totalweight, totallength, totalcount);
        yieldStatis.lot_weight += totalweight / 1000.0f;
        yieldStatis.lot_length += totallength;
        yieldStatis.lot_count += totalcount;

        ctx.circlePos.Statistics(order_no, item_no, melt_no, lot_no, totalweight, totallength, totalcount);
        yieldStatis.lot_weight += totalweight / 1000.0f;
        yieldStatis.lot_length += totallength;
        yieldStatis.lot_count += totalcount;

        // ============ 3. 查询当前班次完成量 ============
        unsigned int err;
        int ban_ci = 0; // 当前班次
        if(!readb(ctx.gplatConn, "SHIFT_NO", &ban_ci, sizeof(ban_ci), &err))
        {
            spdlog::error("Failed to read SHIFT_NO from gPlat (connection={})", ctx.gplatConn);
        }else
        {
            spdlog::info("当前班次: {}", ban_ci);

            //计算当前时间12小时前的时间作为查询条件PRODUCE_TIME的边界
            time_t now = time(nullptr);
            time_t twelveHoursAgo = now - 12 * 60 * 60; // 12小时之前的时间戳
            struct tm twelveHoursAgoTm;
            localtime_r(&twelveHoursAgo, &twelveHoursAgoTm);
            char produce_time_boundary[32];
            strftime(produce_time_boundary, sizeof(produce_time_boundary), "%Y%m%d%H%M%S", &twelveHoursAgoTm);

            //根据合同号、项目号、班次、生产时间查询当前班次的生产完成量
            const pqxx::result result4 = txn.exec(
                "SELECT COALESCE(sum(weight),0), COALESCE(sum(total_length),0), COALESCE(sum(tube),0) "
                "FROM API_BUNDLE_DATA_T "
                "WHERE order_no = $1 AND item_no = $2 AND ban_ci = $3 AND produce_time > $4",
                pqxx::params{order_no, item_no, ban_ci, produce_time_boundary});
            if (!result4.empty())
            {
                const auto &row = result4[0];

                totalweight = row[0].as<float>(0.0f);
                totallength = row[1].as<float>(0.0f);
                totalcount = row[2].as<int>(0);

                yieldStatis.shift_weight = totalweight / 1000.0f; // 转换为吨
                yieldStatis.shift_length = totallength;
                yieldStatis.shift_count = totalcount;

                spdlog::info("当前班次生产完成量: weight={}吨, length={}米, count={}根", yieldStatis.shift_weight, yieldStatis.shift_length, yieldStatis.shift_count);

            }
        }


        // 连接pg数据库更新合同完成量

        txn.exec(
            "UPDATE API_ORDER_DATA_T "
            "SET FINISH_NUMBER = $1, FINISH_LENGTH = $2, FINISH_WEIGHT = $3 "
            "WHERE order_no = $4 AND item_no = $5",
            pqxx::params{yieldStatis.order_count, yieldStatis.order_length, yieldStatis.order_weight * 1000.0f, yieldStatis.order_no.to_string(), yieldStatis.item_no.to_string()});

        txn.commit();

        // 最后将统计出来的信息写入到yieldStatis对象
        // 模拟测试：日志输出最后的yieldStatis对象信息
        spdlog::info("=== Statistics Result ===");
        spdlog::info("Order No: {}", yieldStatis.order_no.to_string());
        spdlog::info("Item No: {}", yieldStatis.item_no.to_string());
        spdlog::info("Melt No: {}", yieldStatis.melt_no.to_string());
        spdlog::info("Lot No: {}", yieldStatis.lot_no.to_string());
        spdlog::info("Order Weight: {}", yieldStatis.order_weight);
        spdlog::info("Order Length: {}", yieldStatis.order_length);
        spdlog::info("Order Count: {}", yieldStatis.order_count);
        spdlog::info("Lot Weight: {}", yieldStatis.lot_weight);
        spdlog::info("Lot Length: {}", yieldStatis.lot_length);
        spdlog::info("Lot Count: {}", yieldStatis.lot_count);
        spdlog::info("Shift Weight: {}", yieldStatis.shift_weight);
        spdlog::info("Shift Length: {}", yieldStatis.shift_length);
        spdlog::info("Shift Count: {}", yieldStatis.shift_count);

        if (writeb(ctx.gplatConn, "YIELD_STATISTICS", &yieldStatis, sizeof(yieldStatis), &err))
        {
            spdlog::info("Successfully wrote gPlat tag YIELD_STATISTICS (connection={}, bytes={})", ctx.gplatConn, sizeof(yieldStatis));

            // 把yieldStatis对象转换为JSON字符串并写入redis的yield_statistics键
            nlohmann::json j;
            j["order_no"] = yieldStatis.order_no.to_string();
            j["item_no"] = yieldStatis.item_no.to_string();
            j["melt_no"] = yieldStatis.melt_no.to_string();
            j["lot_no"] = yieldStatis.lot_no.to_string();
            j["order_weight"] = yieldStatis.order_weight;
            j["order_length"] = yieldStatis.order_length;
            j["order_count"] = yieldStatis.order_count;
            j["lot_weight"] = yieldStatis.lot_weight;
            j["lot_length"] = yieldStatis.lot_length;
            j["lot_count"] = yieldStatis.lot_count;
            j["shift_weight"] = static_cast<int>(yieldStatis.shift_weight * 10) / 10.0;
            j["shift_length"] = static_cast<int>(yieldStatis.shift_length);
            j["shift_count"] = yieldStatis.shift_count;
            std::string jsonStr = j.dump();
            ctx.redis->set("YIELD_STATISTICS", jsonStr);

            // 发布详细消息到RealDataChanged 主题
            ctx.redis->publish("RealDataChanged", "YIELD_STATISTICS");
        }
        else
        {
            spdlog::error(
                "Failed to write gPlat tag YIELD_STATISTICS (connection={}, bytes={}, err={}); verify the deployed gPlat script creates this tag with the current YieldStatistics type",
                ctx.gplatConn,
                sizeof(yieldStatis),
                err);
        }
    }
    catch (const std::exception &e)
    {
        spdlog::error("Database transaction failed: {}", e.what());
    }
}