// ProductionPlan.cpp 投料计划实现
#include "ProductionPlan.h"
#include "TubeTrackContext.h"
#include <memory>
#include <nlohmann/json.hpp>
#include "logging.h" // spdlog

std::unique_ptr<CTube> CProductionPlan::Pop(int /*mode*/)
{
    if (feed_num_ > 0)
    {
        auto tube = std::make_unique<CTube>();
        // 查询当前合同和生产参数，填充管子数据
        tube->order_no = order_no_;
        tube->roll_no = roll_no_;
        tube->item_no = item_no_;
        tube->lot_no = lot_no_;
        tube->melt_no = melt_no_;
        tube->meltno_coupling = meltno_coupling_;
        tube->lotno_coupling = lotno_coupling_;
        tube->tube_no = tube_no_;

        // 更新计数器
        feed_num_--;
        tube_no_ += 10;

        try
        {
            pqxx::work txn(*m_ctx->pgConn);
            txn.exec(
                "UPDATE parameter_set SET feed_number = $1, tube_no = $2",
                pqxx::params{feed_num_, tube_no_});
            txn.commit();
        }
        catch (const std::exception &e)
        {
            spdlog::error("更新parameter_set中的生产计划计数失败: {}", e.what());
            return nullptr;
        }

        UpdateForm();

        return tube;
    }
    else
    {
        //mark 后面要添加报警功能
        spdlog::error("投料支数为0，无法产生管子数据，请设置投料支数！");
        // Program.qbdConnection.LogAlarm("yjg4_Alarm", "投料支数为0，无法产生管子数据，请设置投料支数！", 9);
        return nullptr;
    }
}

bool CProductionPlan::Push(std::unique_ptr<CTube> tube, int /*mode*/)
{
    if (!tube)
    {
        spdlog::error("CProductionPlan::Push失败：传入的管子对象为空");
        return false;
    }

    // 将管子数据写入数据库
    try
    {
        feed_num_++; // 投料支数增加
        pqxx::work txn(*m_ctx->pgConn);
        txn.exec(
            "update parameter_set set feed_number = $1",
            pqxx::params{feed_num_});
        txn.commit();
        spdlog::info("生产参数已写入数据库,feed_number={}", feed_num_);
    }
    catch (const std::exception &e)
    {
        spdlog::error("将管子数据写入数据库失败: {}", e.what());
        return false;
    }

    UpdateForm();

    return true;
}

string CProductionPlan::convertToJson()
{
    // 使用nlohmann/json库实现生产计划转换为JSON格式字符串
    nlohmann::json j;
    j["order_no"] = order_no_;
    j["item_no"] = item_no_;
    j["roll_no"] = roll_no_;
    j["melt_no"] = melt_no_;
    j["lot_no"] = lot_no_;
    j["lotno_coupling"] = lotno_coupling_;
    j["meltno_coupling"] = meltno_coupling_;
    j["feed_num"] = feed_num_;
    j["tube_no"] = tube_no_;

    return j.dump(4);
}

void CProductionPlan::UpdateForm()
{
    try
    {
        // 转化为json格式输出
        string jsonStr = convertToJson();

        // 检查JSON是否有效
        if (jsonStr.empty())
        {
            spdlog::error("错误：生成的JSON字符串为空！");
            return;
        }

        // 写入Redis数据库
        m_ctx->redis->set(m_redisKey, jsonStr);

        // 发布详细消息到 RealDataChanged 主题
        m_ctx->redis->publish("RealDataChanged", m_redisKey);

        spdlog::info("生产计划已更新并发布到Redis: {}", jsonStr);
    }
    catch (const std::exception &e)
    {
        spdlog::error("UpdateForm异常: {}", e.what());
    }
}

void CProductionPlan::ReadParameterSet()
{
    try
    {
        pqxx::nontransaction ntx(*m_ctx->pgConn);
        const pqxx::result result = ntx.exec(
            "SELECT order_no, item_no, roll_no, melt_no, lot_no, "
            "lot_no_coupling, melt_no_coupling, feed_number, tube_no, flow_no "
            "FROM parameter_set "
            "LIMIT 1");

        if (result.empty())
        {
            spdlog::warn("parameter_set表无数据，使用默认生产计划参数");
        }
        else
        {
            const auto &row = result[0];

            this->order_no_ = row["order_no"].as<string>();                // 合同号
            this->item_no_ = row["item_no"].as<string>();                  // 项目号
            this->roll_no_ = row["roll_no"].as<string>();                  // 轧批号
            this->melt_no_ = row["melt_no"].as<string>();                  // 炉号
            this->lot_no_ = row["lot_no"].as<string>();                    // 试批号
            this->lotno_coupling_ = row["lot_no_coupling"].as<string>();   // 接箍批号
            this->meltno_coupling_ = row["melt_no_coupling"].as<string>(); // 接箍炉号
            this->feed_num_ = row["feed_number"].as<int>();                // 投料支数
            this->tube_no_ = row["tube_no"].as<int>();                     // 管号
            this->flow_no_ = row["flow_no"].as<int>();                     // 流水号

            spdlog::info("从数据库加载生产计划参数成功  order_no: {}", this->order_no_);
        }
    }
    catch (const std::exception &e)
    {
        spdlog::error("读取parameter_set失败，使用默认值: {}", e.what());
    }

    UpdateForm(); // 加载参数后更新画面显示
}

// 设置当前合同，更新生产计划参数
bool CProductionPlan::ApplyCurrentContract(const string &orderNo, const string &itemNo)
{
    try
    {
        // 根据合同号、项目号查询批号、外径、壁厚等生产参数
        pqxx::work txn(*m_ctx->pgConn);
        const pqxx::result queryResult = txn.exec(
            "SELECT roll_no, diameter, wall_thickness, weight_per_meter, weight_ew FROM api_order_data_t "
            "WHERE order_no = $1 AND item_no = $2 LIMIT 1",
            pqxx::params{orderNo, itemNo});

        if (queryResult.empty())
        {
            spdlog::warn("No api_order_data_t record found for order_no={}, item_no={}", orderNo, itemNo);
            return false;
        }

        // 更新生产参数表相关字段
        const std::string rollNo = queryResult[0]["roll_no"].as<std::string>("");
        const double Diameter = queryResult[0]["diameter"].as<double>(0.0);
        const double wallThickness = queryResult[0]["wall_thickness"].as<double>(0.0);
        const double weightPerMeter = queryResult[0]["weight_per_meter"].as<double>(0.0);
        const double weightEw = queryResult[0]["weight_ew"].as<double>(0.0);
        const auto updatedRows = txn.exec(
                                        "UPDATE parameter_set SET order_no = $1, item_no = $2, roll_no = $3, diameter = $4, wall_thickness = $5, weight_per_meter = $6, weight_ew = $7",
                                        pqxx::params{orderNo, itemNo, rollNo, Diameter, wallThickness, weightPerMeter, weightEw})
                                     .affected_rows();

        if (updatedRows == 0)
        {
            spdlog::warn("parameter_set update affected 0 rows for order_no={}, item_no={}", orderNo, itemNo);
            return false;
        }

        txn.commit();

        // 更新内部状态并刷新画面
        order_no_ = orderNo;
        item_no_ = itemNo;
        roll_no_ = rollNo;
        UpdateForm();

        spdlog::info(
            "ApplyCurrentContract succeeded: order_no={}, item_no={}, roll_no={}, diameter={}, wall_thickness={}, rows={}",
            orderNo, itemNo, rollNo, Diameter, wallThickness, updatedRows);
        return true;
    }
    catch (const std::exception &e)
    {
        spdlog::error("ApplyCurrentContract failed: {}", e.what());
        return false;
    }
}

void CProductionPlan::RestoreFromRedis()
{
    if (m_ctx && m_ctx->redis)
    {
        // 从Redis获取生产计划数据并恢复
        auto planValue = m_ctx->redis->get(m_redisKey);
        if (planValue)
        {
            RestoreFromJson(*planValue);
        }
    }
}

void CProductionPlan::RestoreFromJson(const string &jsonStr)
{
    try
    {
        if (jsonStr.empty())
        {
            spdlog::warn("{} Redis数据为空，跳过恢复", m_redisKey);
            return;
        }

        nlohmann::json j = nlohmann::json::parse(jsonStr);
        if (!j.is_object() || j.empty())
        {
            spdlog::error("{} Redis数据为空对象或格式错误，跳过恢复", m_redisKey);
            return;
        }

        order_no_ = j.value("order_no", "");
        item_no_ = j.value("item_no", "");
        roll_no_ = j.value("roll_no", "");
        melt_no_ = j.value("melt_no", "");
        lot_no_ = j.value("lot_no", "");
        lotno_coupling_ = j.value("lotno_coupling", "");
        meltno_coupling_ = j.value("meltno_coupling", "");
        feed_num_ = j.value("feed_num", 0);
        tube_no_ = j.value("tube_no", 0);

        spdlog::info("从Redis恢复生产计划成功 order_no: {}", order_no_);
    }
    catch (const std::exception &e)
    {
        spdlog::error("从Redis恢复生产计划失败: {}", e.what());
    }
}

bool CProductionPlan::IsEmpty()
{
    return feed_num_ <= 0;
}
