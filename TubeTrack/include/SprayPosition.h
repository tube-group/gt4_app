// SprayPosition.h 喷印工位类定义
#pragma once
#include "PositionBase.h"
#include "BackBuffer.h"

#include <queue>
#include <chrono>

class CSprayPosition : public CPositionBase
{
public:
    CSprayPosition(string redisKey, string positionName) : CPositionBase(redisKey, positionName)
    {
        spray_length_precision_ = 2; // 默认长度精度为2位小数
        flow_no_ = 0;                // 默认流水号从0开始
        m_backBuffer = nullptr;      // 默认后缓冲区为空
        // 从数据库中读取最后一次喷涂管道的记录时间，用于恢复或初始化喷涂位置的时间状态
        //  多部合一
        // QbdClass.LastSprayTube lastSprayTube = new QbdClass.LastSprayTube();
        // if (Program.qbdConnection.ReadB("yjg4_board1", "LastSprayTube", lastSprayTube))
        // {
        //     m_tEnterTime = lastSprayTube.time; // 使用数据库中保存的最后喷涂时间
        // }
        // else
        // {
        //     m_tEnterTime = DateTime.Now; // 没有历史记录，使用当前时间
        // }
    }

    void UpdateForm() override;                  // 刷新画面
    void ReadParameterSet();                     // 从数据库读取喷印工位参数
    void StartManualLength();                    // 手动测长
    void HandleLengthReady(float actlength);     // 处理测长完成事件
    void StartSpray();                           // 启动喷印
    void SetBackBuffer(CBackBuffer *backBuffer); // 设置后缓冲区对象
    void StartSprayManual();                     // 启动喷印（手动测长）
    void EntryTrigger(const CTube &tube) override;
    bool IsSprayEnable() { return spray_enable_; } // 返回喷印允许状态
    void HandleSprayFinish();                      // 处理喷印完成事件，status表示喷印结果状态

private:
    int length_enable_;          // 测长允许
    int waste_length_enable_;    // 长度判废
    float length_limit_max_;     // 判废管长止
    float length_limit_min_;     // 判废管长起
    float length_coupling_ = 0;  // 保护环长度
    int spray_length_precision_; // 喷涂长度小数位数(长度精度)

    int waste_weight_enable_; // 重量判废
    float weight_limit_max_;  // 管重偏差上限
    float weight_limit_min_;  // 管重偏差下限
    float weight_per_meter_;  // 米重(kg/m)
    float weight_ew_;         // EW值

    int spray_length_type_;   // 喷涂长度格式（0公制1英制）
    int spray_weight_type_;   // 喷涂重量格式（0公制1英制）
    int spray_enable_;        // 喷印允许
    int qrcode_spray_enable_; // 二维码喷印

    int flow_no_; // 喷印工位下一根管子流水号

    std::queue<int> m_flowNoQueue;                      // 保存3个已用过的流水号，用于流水号重复校验
    std::chrono::system_clock::time_point m_tEnterTime; // 管子进入工位时间
    CBackBuffer *m_backBuffer;                          // 后缓冲区对象

    bool PrepairSpray();           // 准备喷印，返回是否允许喷印
    void RecordFlowNo(int flowNo); // 记录最近使用过的流水号
};