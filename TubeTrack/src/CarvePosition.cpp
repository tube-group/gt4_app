// CarvePosition.cpp 刻印工位实现
#include "CarvePosition.h"
#include "TubeTrackContext.h"
#include "logging.h"
#include <ctime>

// 字符串替换工具函数
void CCarvePosition::ReplaceAll(std::string &text, const std::string &from, const std::string &to) const
{
    if (from.empty())
    {
        return;
    }

    size_t pos = 0;
    while ((pos = text.find(from, pos)) != std::string::npos)
    {
        text.replace(pos, from.size(), to);
        pos += to.size();
    }
}
// 刻印参数格式转换
void CCarvePosition::ConvertCarveRequests(CarveReqArray &requests) const
{
    std::time_t now = std::time(nullptr);
    std::tm localTm{};
    localtime_r(&now, &localTm);

    // 计算季度
    int month = localTm.tm_mon + 1;
    std::string quarter;
    if (month >= 1 && month <= 3)
        quarter = "1";
    else if (month >= 4 && month <= 6)
        quarter = "2";
    else if (month >= 7 && month <= 9)
        quarter = "3";
    else
        quarter = "4";
    // 计算年份最后一位
    int year = localTm.tm_year + 1900;
    std::string yearToken = std::to_string(year % 10);
    // 计算月份
    std::string monthStr = std::to_string(month);

    for (std::string &request : requests)
    {
        ReplaceAll(request, "季", quarter);
        ReplaceAll(request, "年", yearToken);
        ReplaceAll(request, "月", monthStr);
    }
}
// 从数据库加载刻印参数
bool CCarvePosition::LoadCarveRequests(const CTube &tube, CarveReqArray &requests) const
{
    try
    {
        pqxx::nontransaction ntx(*m_ctx->pgConn);
        const pqxx::result result = ntx.exec(
            "SELECT stamp_req_1_manual, stamp_req_2_manual, stamp_req_3_manual, stamp_req_4_manual, "
            "stamp_req_5_manual, stamp_req_6_manual, stamp_req_7_manual, stamp_req_8_manual "
            "FROM api_order_data_t "
            "WHERE order_no = $1 AND item_no = $2 ",
            pqxx::params{tube.order_no, tube.item_no});

        if (result.empty())
        {
            spdlog::warn("未找到刻印参数，order_no={}, item_no={}", tube.order_no, tube.item_no);
            return false;
        }

        const auto &row = result[0];
        requests[0] = row["stamp_req_1_manual"].as<std::string>("");
        requests[1] = row["stamp_req_2_manual"].as<std::string>("");
        requests[2] = row["stamp_req_3_manual"].as<std::string>("");
        requests[3] = row["stamp_req_4_manual"].as<std::string>("");
        requests[4] = row["stamp_req_5_manual"].as<std::string>("");
        requests[5] = row["stamp_req_6_manual"].as<std::string>("");
        requests[6] = row["stamp_req_7_manual"].as<std::string>("");
        requests[7] = row["stamp_req_8_manual"].as<std::string>("");
        return true;
    }
    catch (const std::exception &e)
    {
        spdlog::error("读取刻印参数失败，order_no={}, item_no={}, error={}", tube.order_no, tube.item_no, e.what());
        return false;
    }
}
// 将刻印参数发送给PLC
bool CCarvePosition::SendCarveRequestsToPlc(const CarveReqArray &requests) const
{
    if (m_ctx == nullptr)
    {
        spdlog::error("刻印工位上下文为空，无法下发PLC");
        return false;
    }

    if (m_ctx->gplatConn < 0)
    {
        spdlog::error("刻印工位PLC连接不可用，gplatConn={}", m_ctx->gplatConn);
        return false;
    }

    unsigned int error = 0;
    const std::array<std::pair<const char *, std::string>, 2> writeRequests{{
        {"CARVE_PARA1", requests[0]}, // 第1行刻印
        {"CARVE_PARA2", requests[1]}, // 第2行刻印
    }};

    // 依次写入刻印参数到PLC
    for (const auto &[tag, value] : writeRequests)
    {
        if (write_plc_string(m_ctx->gplatConn, tag, value, &error))
        {
            spdlog::info("写入刻印字符串成功，tag={}, value={}", tag, value);
        }
        else
        {
            spdlog::error("写入刻印字符串失败，tag={}, value={}, error={}", tag, value, error);
            return false;
        }
    }

    // 写入刻印启动位,触发刻印启动
    if (write_plc_bool(m_ctx->gplatConn, "STAMP_START", true, &error))
    {
        spdlog::info("写入刻印启动位成功，tag=STAMP_START");
    }
    else
    {
        spdlog::error("写入刻印启动位失败，tag=STAMP_START, error={}", error);
        return false;
    }

    spdlog::info(
        "刻印数据已下发到PLC req1='{}', req2='{}', req3='{}'",
        requests[0],
        requests[1],
        requests[2]);

    return true;
}

// 刷新刻印工位的界面显示
void CCarvePosition::UpdateForm()
{
    if (m_ctx && m_ctx->redis)
    {
        m_ctx->redis->set(m_redisKey, convertToJson());
        spdlog::info("{}: {} updated", m_positionName, m_redisKey);

        // 发布详细消息到 RealDataChanged 主题
        m_ctx->redis->publish("RealDataChanged", m_redisKey);
    }
}

// 从数据库读取刻印工位参数
void CCarvePosition::ReadParameterSet()
{
    try
    {
        pqxx::nontransaction ntx(*m_ctx->pgConn);
        const pqxx::result result = ntx.exec(
            "SELECT carve_enable "
            "FROM parameter_set "
            "LIMIT 1");
        if (result.empty())
        {
            spdlog::warn("parameter_set表无数据，使用默认刻印工位参数");
        }
        else
        {
            const auto &row = result[0];
            this->carve_enable_ = row["carve_enable"].as<int>(); // 刻印允许

            spdlog::info("刻印工位从数据库加载生产计划参数成功");

            unsigned int err;
            write_plc_bool(m_ctx->gplatConn, "CARVE_ENABLE", carve_enable_ != 0, &err);
        }
    }
    catch (const std::exception &e)
    {
        spdlog::error("读取parameter_set失败，使用默认值: {}", e.what());
    }
}

// 管子push进入刻印工位后触发刻印功能
void CCarvePosition::EntryTrigger(const CTube &tube)
{
    unsigned int err;
    bool status = tube.length_ok && tube.weight_ok;
    write_plc_bool(m_ctx->gplatConn, "CARVE_ENABLE", status, &err);
    spdlog::info("通知PLC刻印工位的管子是否为废管: status={}", status);

    // 标记不合格管子————前向工位里管子数据被手动修改为不合格
    if (!tube.length_ok || !tube.weight_ok)
    {
        ReleaseWB(); // 释放步进梁封锁，允许废管通过
        spdlog::warn("刻印工位检测到不合格管子，直接放行");
        return;
    }

    // 检查是否需要刻印
    // bool needCarve = carve_enable_ && IsOccupied();
    bool needCarve = carve_enable_;
    if (!needCarve)
    {
        ReleaseWB();
        spdlog::info("刻印未使能或无料，释放步进梁");
        return;
    }

    // 执行刻印
    spdlog::info("开始刻印流程");

    // 创建刻印参数数组，并且初始化为空字符串
    CarveReqArray carveRequests{};

    // 从数据库表api_order_data_t读取刻印参数
    LoadCarveRequests(tube, carveRequests);

    // 读取的参数进行格式转换(针刻印格式设定)
    ConvertCarveRequests(carveRequests);

    // 将转换后的针刻印数据发送给PLC
    if (SendCarveRequestsToPlc(carveRequests))
    {
        spdlog::info("发送PLC成功");
    }
    else
    {
        spdlog::error("发送PLC失败");
        // 如果下发PLC失败，记录错误日志并释放步进梁封锁，允许管子通过
        ReleaseWB();
    }

    BlockWB();
    spdlog::info("刻印数据发送完成，步进梁已封锁,等待刻印完成信号");
    
}

// 处理刻印完成后的逻辑
void CCarvePosition::HandleCarveFinish()
{
    ReleaseWB(); // 释放步进梁封锁
}
