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

            this->length_enable_ = row["length_enable"].as<int>();                   // 测长允许
            this->waste_length_enable_ = row["waste_length_enable"].as<int>();       // 长度判废
            this->length_limit_max_ = row["length_limit_max"].as<float>();           // 判废管长止
            this->length_limit_min_ = row["length_limit_min"].as<float>();           // 判废管长起
            this->length_coupling_ = row["length_coupling"].as<float>();             // 保护环长度
            this->spray_length_precision_ = row["spray_length_precision"].as<int>(); // 喷涂长度小数位数
            this->spray_length_type_ = row["spray_length_type"].as<int>();           // 喷涂长度格式（0公制1英制）
            this->spray_weight_type_ = row["spray_weight_type"].as<int>();           // 喷涂重量格式（0公制1英制）
            this->spray_enable_ = row["spray_enable"].as<int>();                     // 喷印允许
            this->qrcode_spray_enable_ = row["qrcode_spray_enable"].as<int>();       // 二维码喷印
            this->flow_no_ = row["flow_no"].as<int>();                               // 喷印工位下一根管子流水号

            spdlog::info("喷印工位从数据库加载生产计划参数成功");
        }
    }
    catch (const std::exception &e)
    {
        spdlog::error("读取parameter_set失败，使用默认值: {}", e.what());
    }
}

// //---------------启动手动测长-------------------------
// void CSprayPosition::StartManualLength()
// {
//     // 标记手动测长状态，等待人工输入长度数据
//     // m_bManualLength = true;
//     write_plc_bool(m_ctx->gplatConn, "[S7_GT4_1200]DB12,X0.7", true, &error);
// }

// //---------------处理测长完成事件-------------------------
// void CSprayPosition::HandleLengthReady(float actlength)
// {
//     // 获取该工位的管子对象
//     CTube *tube = const_cast<CTube *>(Peek());

//     // 测长允许
//     if (length_enable_)
//     {
//         // 处理长度数据
//         float length = actlength - length_coupling_; // 喷印长度=实际长度-保护环长度
//         // 按指定精度四舍五入
//         length = std::round(length * std::pow(10, spray_length_precision_)) / std::pow(10, spray_length_precision_);
//         if (tube)
//         {
//             tube->length = length;

//             // 长度判废
//             if (waste_length_enable_)
//             {
//                 if (length < length_limit_min_ || length > length_limit_max_)
//                 {
//                     tube->length_ok = false; // 长度不合格
//                     m_bWbReleased = false;   // 封锁步进梁，等待人工处理
//                 }
//                 else
//                 {
//                     tube->length_ok = true; // 长度合格
//                 }
//             }
//             else
//             {
//                 tube->length_ok = true; // 不判废默认长度合格
//             }

//             // 重量判废
//             if (waste_weight_enable_)
//             {
//                 // 只有长度合格才判断重量
//                 if (tube->length_ok)
//                 {
//                     // 计算管重偏差
//                     float expected_weight = tube->length * weight_per_meter_ + weight_ew_;
//                     ;                                                            // 计算理论重量
//                     float weightMax = expected_weight * (1 + weight_limit_max_); // 重量上限
//                     float weightMin = expected_weight * (1 - weight_limit_min_); // 重量下限

//                     if (tube->weight < weightMin || tube->weight > weightMax)
//                     {
//                         tube->weight_ok = false; // 重量不合格
//                         m_bWbReleased = false;   // 封锁步进梁，等待人工处理
//                     }
//                     else
//                     {
//                         tube->weight_ok = true; // 重量合格
//                     }
//                 }
//                 else
//                 {
//                     tube->weight_ok = true; // 长度不合格不判断重量，默认重量合格
//                 }
//             }
//             else
//             {
//                 tube->weight_ok = true; // 不判废默认重量合格
//             }
//         }
//     }
//     else
//     {
//         if (tube)
//         {
//             tube->length = 0.0f;    // 测长不允许，长度设为0
//             tube->length_ok = true; // 长度合格
//         }
//     }

//     unsigned int error;
//     // 根据判定结果处理废管或合格管
//     if (tube && (!tube->length_ok || !tube->weight_ok))
//     {
//         // 废管处理
//         spdlog::warn("废管不喷印！");
//         write_plc_bool(m_ctx->gplatConn, "[S7_GT4_1200]DB12,X0.2", true, &error);          // 废管标志
//         write_plc_string(m_ctx->gplatConn, "[S7_GT4_1200]DB12,STRING6.254", "", &error);   // 喷印字符串清空
//         write_plc_string(m_ctx->gplatConn, "[S7_GT4_1200]DB12,STRING518.254", "", &error); // 发送二维码喷印内容
//         write_plc_bool(m_ctx->gplatConn, "[S7_GT4_1200]DB12,X0.1", false, &error);         // 喷印完成
//         write_plc_bool(m_ctx->gplatConn, "[S7_GT4_1200]DB12,X1.2", false, &error);         // 条码喷印完成
//         write_plc_bool(m_ctx->gplatConn, "[S7_GT4_1200]DB12,X1.4", false, &error);         // 管体喷印完成
//         write_plc_bool(m_ctx->gplatConn, "[S7_GT4_1200]DB12,X0.0", true, &error);          // 发送标志位置位
//         std::this_thread::sleep_for(std::chrono::milliseconds(1000));
//         write_plc_bool(m_ctx->gplatConn, "[S7_GT4_1200]DB12,X1.3", true, &error); // 启动管体喷印
//         write_plc_bool(m_ctx->gplatConn, "[S7_GT4_1200]DB12,X1.1", true, &error); // 启动条码喷印
//     }
//     else
//     {
//         // 合格管处理
//         write_plc_bool(m_ctx->gplatConn, "[S7_GT4_1200]DB12,X0.2", false, &error); // 废管标志
//         if (PrepairSpray())
//         {
//             StartSpray(); // 启动喷印
//         }
//     }

//     m_bManualLength = false; // 重置手动测长状态

//     UpdateForm(); // 刷新画面
// }

// //---------------准备喷印-------------------------
// bool CSprayPosition::PrepairSpray()
// {
//     // 喷印不允许，释放步进梁
//     if (!spray_enable_)
//     {
//         m_bWbReleased = true;
//     }
//     m_tEnterTime = std::chrono::system_clock::now(); // 记录管子进入工位的时间点

//     // // 多部合一
//     // QbdClass.LastSprayTube lastSprayTube = new QbdClass.LastSprayTube();
//     // lastSprayTube.time = DateTime.Now;
//     // Program.qbdConnection.WriteB("yjg4_board1", "LastSprayTube", lastSprayTube);

//     // 获取该工位的管子对象
//     CTube *tube = const_cast<CTube *>(Peek());

//     // 管子不为空且管子长度合格且重量合格才允许喷印
//     if (tube && tube->length_ok && tube->weight_ok)
//     {
//         // 流水号已存在且已喷印过，说明是重复流水号，禁止喷印
//         if (tube->flow_no != 0 && tube->sprayed)
//         {
//             m_bWbReleased = true; // 释放步进梁
//             spdlog::error("流水号{}重复且已喷印过，禁止喷印！请确认是否需要人工补喷", tube->flow_no);
//             return false; // 不允许喷印
//         }

//         // 生成流水号
//         if (tube->flow_no == 0 && tube->length_ok && tube->weight_ok) // 只有在流水号为0时才生成新流水号
//         {
//             flow_no_++;
//             tube->flow_no = flow_no_;

//             // 将新流水号加入队列
//             m_flowNoQueue.push(flow_no_);
//             if (m_flowNoQueue.size() > 3) // 队列超过3个元素时，移除最旧的流水号
//             {
//                 m_flowNoQueue.pop();
//             }
//         }
//     }
//     else
//     {
//         return false; // 不允许喷印
//     }
//     return true; // 允许喷印
// }

// //---------------启动喷印-------------------------
// void CSprayPosition::StartSpray()
// {

// }

// //---------------启动人工喷印-------------------------
// void CSprayPosition::StartSprayManual()
// {

// }

// //---------------重写PushBack方法，将管子推入后缓冲区-------------------------
// bool CSprayPosition::PushBack(unique_ptr<CTube> tube, int mode)
// {
//     return true;  // 添加 return
// }

// //---------------处理喷印完成事件，status表示喷印结果状态-------------------------
// void CSprayPosition::HandleSprayFinish(int status)
// {
    
// }