// SprayPosition.cpp 喷印工位实现
#include "SprayPosition.h"
#include "TubeTrackContext.h"
#include "logging.h"

void CSprayPosition::UpdateForm()
{
    // 刷新喷印工位的界面显示
    if (m_ctx && m_ctx->redis)
    {
        m_ctx->redis->set(m_redisKey, convertToJson());
        spdlog::info("{}: {} updated", m_positionName, m_redisKey);

        // 发布详细消息到 RealDataChanged 主题
        m_ctx->redis->publish("RealDataChanged", m_redisKey);
    }
}

void CSprayPosition::ReadParameterSet()
{
    try
    {
        pqxx::nontransaction ntx(*m_ctx->pgConn);
        const pqxx::result result = ntx.exec(
            "SELECT length_enable, waste_length_enable, length_limit_max, length_limit_min, length_coupling, "
            "spray_length_precision, spray_length_type, spray_weight_type, spray_enable, qrcode_spray_enable, flow_no "
            "FROM parameter_set "
            "LIMIT 1");
        if (result.empty())
        {
            spdlog::warn("parameter_set表无数据，使用默认喷印工位参数");
        }
        else
        {
            const auto &row = result[0];

            this->length_enable_ = row["length_enable"].as<int>();             // 测长允许
            this->waste_length_enable_ = row["waste_length_enable"].as<int>(); // 长度判废
            this->length_limit_max_ = row["length_limit_max"].as<float>();     // 判废管长止
            this->length_limit_min_ = row["length_limit_min"].as<float>();     // 判废管长起
            this->length_coupling_ = row["length_coupling"].as<float>();      // 保护环长度
            this->spray_length_precision_ = row["spray_length_precision"].as<int>(); // 喷涂长度小数位数
            this->spray_length_type_ = row["spray_length_type"].as<int>();    // 喷涂长度格式（0公制1英制）
            this->spray_weight_type_ = row["spray_weight_type"].as<int>();    // 喷涂重量格式（0公制1英制）
            this->spray_enable_ = row["spray_enable"].as<int>();              // 喷印允许
            this->qrcode_spray_enable_ = row["qrcode_spray_enable"].as<int>(); // 二维码喷印
            this->flow_no_ = row["flow_no"].as<int>();                        // 喷印工位下一根管子流水号

            spdlog::info("喷印工位从数据库加载生产计划参数成功");
        }
    }
    catch (const std::exception &e)
    {
        spdlog::error("读取parameter_set失败，使用默认值: {}", e.what());
    }
}