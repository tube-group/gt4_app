// SprayPosition.cpp 喷印工位实现
#include "SprayPosition.h"
#include "TubeTrackContext.h"
#include "logging.h"
#include <cmath>

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
            "waste_weight_enable, weight_limit_max, weight_limit_min, weight_per_meter, weight_ew, "
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
            this->waste_weight_enable_ = row["waste_weight_enable"].as<int>();       // 重量判废
            this->weight_limit_max_ = row["weight_limit_max"].as<float>();           // 管重偏差上限
            this->weight_limit_min_ = row["weight_limit_min"].as<float>();           // 管重偏差下限
            this->weight_per_meter_ = row["weight_per_meter"].as<float>();           // 米重(kg/m)
            this->weight_ew_ = row["weight_ew"].as<float>();                         // EW值
            this->spray_length_precision_ = row["spray_length_precision"].as<int>(); // 喷涂长度小数位数
            this->spray_length_type_ = row["spray_length_type"].as<int>();           // 喷涂长度格式（0公制1英制）
            this->spray_weight_type_ = row["spray_weight_type"].as<int>();           // 喷涂重量格式（0公制1英制）
            this->spray_enable_ = row["spray_enable"].as<int>();                     // 喷印允许
            this->qrcode_spray_enable_ = row["qrcode_spray_enable"].as<int>();       // 二维码喷印
            this->flow_no_ = row["flow_no"].as<int>();                               // 喷印工位下一根管子流水号

            // 将下一根管子的流水号写入Redis，NEXT_TUBE_FLOW_NO键用于前端显示
            m_ctx->redis->set("NEXT_TUBE_FLOW_NO", std::to_string(flow_no_));
            spdlog::info("喷印工位下一根管子流水号已写入Redis: NEXT_TUBE_FLOW_NO={}", flow_no_);
            m_ctx->redis->publish("RealDataChanged", "NEXT_TUBE_FLOW_NO");

            unsigned int error = 0;
            write_plc_bool(m_ctx->gplatConn, "COUP_DISABLE", spray_enable_ != 0, &error);
            write_plc_bool(m_ctx->gplatConn, "STAMP_DISABLE", qrcode_spray_enable_ != 0, &error);

            spdlog::info("喷印工位从数据库加载生产计划参数成功");
        }
    }
    catch (const std::exception &e)
    {
        spdlog::error("读取parameter_set失败，使用默认值: {}", e.what());
    }
}

//---------------启动手动测长-------------------------
void CSprayPosition::StartManualLength()
{
    // 标记手动测长状态，等待人工输入长度数据
    m_bManualLength = true;
    unsigned int error;
    write_plc_bool(m_ctx->gplatConn, "LENGTH_START", true, &error);
}

//---------------处理测长完成事件-------------------------
void CSprayPosition::HandleLengthReady(float actlength)
{
    // 获取该工位的管子对象
    CTube *tube = const_cast<CTube *>(Peek());

    // 测长允许
    if (length_enable_)
    {
        // 处理长度数据
        float length = actlength / 1000.0f - length_coupling_; // 喷印长度=实际长度-保护环长度
        // 按指定精度四舍五入
        length = std::round(length * std::pow(10, spray_length_precision_)) / std::pow(10, spray_length_precision_);
        if (tube)
        {
            tube->length = length;

            spdlog::info("管子流水号{}测得长度{}m，保护环长度{}m，喷印长度{}m", tube->flow_no, actlength / 1000.0f, length_coupling_, tube->length);
            // 长度判废
            if (waste_length_enable_)
            {
                spdlog::info("长度判废启用，长度合格范围：{}m ~ {}m", length_limit_min_, length_limit_max_);
                if (length < length_limit_min_ || length > length_limit_max_)
                {
                    tube->length_ok = false; // 长度不合格
                    m_bWbReleased = false;   // 封锁步进梁，等待人工处理
                }
                else
                {
                    tube->length_ok = true; // 长度合格
                }
            }
            else
            {
                tube->length_ok = true; // 不判废默认长度合格
            }

            // 重量判废
            if (waste_weight_enable_)
            {
                spdlog::info("重量判废启用，米重{}kg/m，EW值{}kg，重量偏差范围：{}% ~ {}%", weight_per_meter_, weight_ew_, weight_limit_min_, weight_limit_max_);
                // 只有长度合格才判断重量
                if (tube->length_ok)
                {
                    // 计算管重偏差
                    float expected_weight = tube->length * weight_per_meter_ + weight_ew_; // 计算理论重量
                    float weightMax = expected_weight * (1 + weight_limit_max_ / 100);     // 重量上限
                    float weightMin = expected_weight * (1 + weight_limit_min_ / 100);     // 重量下限

                    spdlog::info("管子流水号{}理论重量{}kg，实际重量{}kg，重量合格范围：{}kg ~ {}kg", tube->flow_no, expected_weight, tube->weight, weightMin, weightMax);
                    if (tube->weight < weightMin || tube->weight > weightMax)
                    {
                        tube->weight_ok = false; // 重量不合格
                        m_bWbReleased = false;   // 封锁步进梁，等待人工处理
                    }
                    else
                    {
                        tube->weight_ok = true; // 重量合格
                    }
                }
                else
                {
                    tube->weight_ok = false; // 长度不合格不判断重量，默认重量不合格
                }

                // debug: 测试环境下，一律合格
                //  tube->weight_ok = true;
                //  m_bWbReleased = true;
            }
            else
            {
                tube->weight_ok = true; // 不判废默认重量合格
            }
        }
    }
    else
    {
        if (tube)
        {
            tube->length = 0.0f;    // 测长不允许，长度设为0
            tube->length_ok = true; // 长度合格
        }
    }

    unsigned int error;
    // 根据判定结果处理废管或合格管
    if (tube && (!tube->length_ok || !tube->weight_ok))
    {
        // 废管处理
        spdlog::warn("废管不喷印！");
        write_plc_bool(m_ctx->gplatConn, "SPRAY_WASTE_FLAG", true, &error);          // 废管标志
        // write_plc_string(m_ctx->gplatConn, "SPRAY_STRING_TO_L1", "", &error);   // 喷印字符串清空
        // write_plc_string(m_ctx->gplatConn, "BARCODE_STRING_TO_L1", "", &error); // 发送二维码喷印内容
        write_plc_bool(m_ctx->gplatConn, "SPRAY_FINISH_NOUSE", false, &error);         // 喷印完成
        write_plc_bool(m_ctx->gplatConn, "QUICK_MARK_FINISH", false, &error);         // 条码喷印完成
        write_plc_bool(m_ctx->gplatConn, "SPRAY_FINISH", false, &error);         // 管体喷印完成
        write_plc_bool(m_ctx->gplatConn, "SPRAY_START_NOUSE", true, &error);          // 发送标志位置位
        std::this_thread::sleep_for(std::chrono::milliseconds(1000));
        write_plc_bool(m_ctx->gplatConn, "SPRAY_START", true, &error); // 启动管体喷印
        write_plc_bool(m_ctx->gplatConn, "QUICK_MARK_START", true, &error); // 启动条码喷印
        spdlog::info("流水号{}的管子长度{}m，重量{}kg，判定为废管", tube->flow_no, tube->length, tube->weight);
    }
    else
    {
        // 合格管处理
        write_plc_bool(m_ctx->gplatConn, "SPRAY_WASTE_FLAG", false, &error); // 废管标志
        if (PrepairSpray())
        {
            StartSpray(); // 启动喷印
        }
    }

    m_bManualLength = false; // 重置手动测长状态

    UpdateForm(); // 刷新画面
}

//---------------准备喷印-------------------------
bool CSprayPosition::PrepairSpray()
{
    // 喷印不允许，释放步进梁
    if (!spray_enable_)
    {
        m_bWbReleased = true;
    }
    m_tEnterTime = std::chrono::system_clock::now(); // 记录管子进入工位的时间点

    // 将当前时间写入数据库，记录最后一次喷涂管道的发生时间
    // // 多部合一
    // QbdClass.LastSprayTube lastSprayTube = new QbdClass.LastSprayTube();
    // lastSprayTube.time = DateTime.Now;
    // Program.qbdConnection.WriteB("yjg4_board1", "LastSprayTube", lastSprayTube);

    // 获取该工位的管子对象
    CTube *tube = const_cast<CTube *>(Peek());

    // 管子不为空且管子长度合格且重量合格才允许喷印
    if (tube && tube->length_ok && tube->weight_ok)
    {
        // 流水号已存在且已喷印过，说明是重复流水号，禁止喷印
        if (tube->flow_no != 0 && tube->sprayed)
        {
            m_bWbReleased = false; // 封锁步进梁，等待人工处理
            spdlog::error("流水号{}重复且已喷印过，禁止喷印！请确认是否需要人工补喷", tube->flow_no);
            return false; // 不允许喷印
        }

        // 生成流水号
        if (tube->flow_no == 0 && tube->length_ok && tube->weight_ok) // 只有在流水号为0时才生成新流水号
        {
            tube->flow_no = flow_no_;
            flow_no_++;
            
            // 将生成的流水号写入数据库，更新喷印工位下一根管子流水号
            try
            {
                pqxx::work txn(*m_ctx->pgConn);
                txn.exec("UPDATE parameter_set SET flow_no = $1", pqxx::params{flow_no_});
                txn.commit();
                spdlog::info("更新数据库中喷印工位下一根管子流水号成功，flow_no={}", flow_no_);

                // 将下一根管子的流水号写入Redis，NEXT_TUBE_FLOW_NO键用于前端显示
                m_ctx->redis->set("NEXT_TUBE_FLOW_NO", std::to_string(flow_no_));
                m_ctx->redis->publish("RealDataChanged", "NEXT_TUBE_FLOW_NO");
            }
            catch (const std::exception &e)
            {
                spdlog::error("更新数据库中喷印工位下一根管子流水号失败: {}", e.what());
            }
        }

        if (spray_enable_ && !tube->sprayed)
        {
            return true;
        }
        spdlog::info("流水号{}已经喷印", flow_no_);
    }

    return false;
}

//---------------启动喷印-------------------------
void CSprayPosition::StartSpray()
{
    CTube *tube = const_cast<CTube *>(Peek());
    if (!tube || spray_enable_ == 0)
    {
        return;
    }

    if (tube->flow_no == 0 && tube->length_ok && tube->weight_ok)
    {
        m_bWbReleased = false; // 封锁步进梁，等待人工处理
        spdlog::error("流水号为0，禁止喷印！请确认是否需要人工补喷");
        return;
    }

    // 合格管子的重复校验
    if (tube->length_ok && tube->weight_ok)
    {
        bool flowNoRepeated = false;

        // 第一层:检查最近3个流水号队列
        std::queue<int> tempQueue = m_flowNoQueue; // 创建一个临时队列来检查流水号
        while (!tempQueue.empty())
        {
            if (tube->flow_no == tempQueue.front())
            {
                flowNoRepeated = true;
                m_bWbReleased = false;
                spdlog::error("流水号{}重复", tube->flow_no);
                std::string temp = "流水号" + std::to_string(tube->flow_no) + "刚刚使用过，请重新选择一个新的流水号";
                spdlog::error(temp);
                break;
            }
            tempQueue.pop();
        }
        if (flowNoRepeated)
        {
            return; // 流水号重复，禁止喷印
        }

        // 第二层：检查缓冲区中是否存在相同流水号的管子
        if (m_backBuffer && m_backBuffer->QueryTube(tube->order_no, tube->flow_no))
        {
            flowNoRepeated = true;
            m_bWbReleased = false;
            spdlog::error("缓冲区中存在相同流水号的管子，请重新指定一个新的流水号");
        }
        if (flowNoRepeated)
        {
            return; // 流水号重复，禁止喷印
        }

        // 第三层：查找管子表进一步判断流水号是否已使用
        pqxx::nontransaction ntx(*m_ctx->pgConn);

        // 查询历史表/*, produce_time*/
        const pqxx::result result = ntx.exec(
            "SELECT bundle_no FROM api_tube_data_t "
            "WHERE order_no = $1 AND flow_no = $2",
            pqxx::params{tube->order_no, tube->flow_no});

        if (!result.empty())
        {
            const auto &row = result[0];
            std::string bundle_no = row["bundle_no"].as<std::string>();

            flowNoRepeated = true;
            m_bWbReleased = false;

            spdlog::error("流水号{}重复", tube->flow_no);
            std::string temp = "流水号" + std::to_string(tube->flow_no) +
                               "在管捆" + bundle_no + "中已经使用，请重新指定一个新的流水号";
            spdlog::error(temp);
        }

        if (flowNoRepeated)
        {
            return; // 流水号重复，禁止喷印
        }
    }

    // 生成喷印字符串发送给喷印进程
    unsigned int error = 0;
    StartSprayEvent event;
    event.order_no = tube->order_no;
    event.item_no = tube->item_no;
    event.tube_no = tube->tube_no;
    event.flow_no = tube->flow_no;
    event.melt_no = tube->melt_no;
    event.lot_no = tube->lot_no;
    event.length = tube->length;
    event.weight = tube->weight;
    event.theory_weight = tube->length * weight_per_meter_ + weight_ew_;
    event.length_ok = tube->length_ok;
    event.weight_ok = tube->weight_ok;
    bool ret = writeb(m_ctx->gplatConn, "START_SPRAY_EVENT", &event, sizeof(event), &error);
    spdlog::info("发送喷印事件: order_no={}, item_no={}, tube_no={}, flow_no={}, melt_no={}, lot_no={}, length={}, weight={}, theory_weight={}, length_ok={}, weight_ok={}, ret={}, error={}",
                 event.order_no.c_str(),
                 event.item_no.c_str(),
                 event.tube_no,
                 event.flow_no,
                 event.melt_no.c_str(),
                 event.lot_no.c_str(),
                 event.length,
                 event.weight,
                 event.theory_weight,
                 event.length_ok,
                 event.weight_ok,
                 ret,
                 error);
}

//---------------启动人工喷印-------------------------
void CSprayPosition::StartSprayManual()
{
    const CTube *tube = Peek();
    if (!tube || spray_enable_ == 0)
    {
        return;
    }
    if (tube->flow_no == 0)
    {
        m_bWbReleased = false; // 封锁步进梁，等待人工处理
        spdlog::error("流水号为0，禁止喷印！");
        return;
    }
    if (!tube->length_ok && !tube->weight_ok)
    {
        m_bWbReleased = false; // 封锁步进梁，等待人工处理
        spdlog::error("废管禁止喷印！");
        return;
    }

    // 生成喷印字符串发送给喷印进程
    unsigned int error = 0;
    StartSprayEvent event;
    event.order_no = tube->order_no;
    event.item_no = tube->item_no;
    event.tube_no = tube->tube_no;
    event.flow_no = tube->flow_no;
    event.melt_no = tube->melt_no;
    event.lot_no = tube->lot_no;
    event.length = tube->length;
    event.weight = tube->weight;
    event.theory_weight = tube->length * weight_per_meter_ + weight_ew_;
    event.length_ok = tube->length_ok;
    event.weight_ok = tube->weight_ok;
    bool ret = writeb(m_ctx->gplatConn, "START_SPRAY_EVENT", &event, sizeof(event), &error);
    spdlog::info("发送喷印事件: order_no={}, item_no={}, tube_no={}, flow_no={}, melt_no={}, lot_no={}, length={}, weight={}, theory_weight={}, length_ok={}, weight_ok={}, ret={}, error={}",
                 event.order_no.c_str(),
                 event.item_no.c_str(),
                 event.tube_no,
                 event.flow_no,
                 event.melt_no.c_str(),
                 event.lot_no.c_str(),
                 event.length,
                 event.weight,
                 event.theory_weight,
                 event.length_ok,
                 event.weight_ok,
                 ret,
                 error);
}

//---------------工位入口触发事件-------------------------
void CSprayPosition::EntryTrigger(const CTube &tube)
{
    // 管子进入工位，记录进入时间，处理废管状态，并根据废管状态自动释放或封锁步进梁
    m_bManualLength = false; // 重置手动测长状态
    // 记录进入时间（合格管和废管都需要记录）
    m_tEnterTime = std::chrono::system_clock::now();
    // 废管处理
    if (!tube.length_ok || !tube.weight_ok)
    {
        m_bWbReleased = true;
        spdlog::warn("废管进入，自动释放，流水号: {}", tube.flow_no);
    }
}

//---------------处理喷印完成事件，status表示喷印结果状态-------------------------
void CSprayPosition::HandleSprayFinish(int status)
{
    m_bManualLength = false;
    if (status == 0) // 喷印成功
    {
        m_bWbReleased = true;
    }
    else
    {
        m_bWbReleased = false; // 喷印失败，封锁步进梁，等待人工处理
    }
}

//---------------设置后缓冲区对象-------------------------
void CSprayPosition::SetBackBuffer(CBackBuffer *backBuffer)
{
    m_backBuffer = backBuffer;
}

//---------------记录最近使用过的流水号-------------------------
void CSprayPosition::RecordFlowNo(int flowNo)
{

    if (flowNo <= 0)
    {
        return;
    }

    m_flowNoQueue.push(flowNo); // 添加新的流水号
    while (m_flowNoQueue.size() > 10)
    {
        m_flowNoQueue.pop(); // 移除最旧的流水号
    }
}