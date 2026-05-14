// SprayPosition.h 喷印工位类定义
#pragma once
#include "PositionBase.h"
#include "BackBuffer.h"

#include <queue>
#include <mutex>
#include <chrono>

class CSprayPosition : public CPositionBase
{
public:
    CSprayPosition(string redisKey, string positionName) : CPositionBase(redisKey, positionName)
    {
        spray_length_precision_ = 2; // 默认长度精度为2位小数
        // m_bManualLength = false; // 默认自动测长
        flow_no_ = 0;                        // 默认流水号从0开始
        // m_flowNoQueue = std::queue<int>(10); // 初始化流水号队列

        // // 多部合一
        // QbdClass.LastSprayTube lastSprayTube = new QbdClass.LastSprayTube();
        // if (Program.qbdConnection.ReadB("yjg4_board1", "LastSprayTube", lastSprayTube))
        // {
        //     m_tEnterTime = lastSprayTube.time;
        // }
        // else
        // {
        //     m_tEnterTime = DateTime.Now;
        // }
    }

    void UpdateForm() override;                                               // 刷新画面
    void ReadParameterSet();                                                  // 从数据库读取喷印工位参数
    // void StartManualLength();                                                 // 手动测长
    // void HandleLengthReady(float actlength);                                  // 处理测长完成事件
    // bool PrepairSpray();                                                      // 准备喷印，返回是否允许喷印
    // void StartSpray();                                                        // 启动喷印
    // void StartSprayManual();                                                  // 启动喷印（手动测长）
    // // void SetBackBuffer(CBackBuffer backBuffer) { m_backBuffer = backBuffer; } // 设置后缓冲区对象
    // bool PushBack(unique_ptr<CTube> tube, int mode = 0) override;             // 重写PushBack方法，将管子推入后缓冲区
    // bool IsSprayEnable() { return spray_enable_; }                            // 返回喷印允许状态
    // // DateTime EnterTime() { return m_tEnterTime; }                             // 返回管子进入工位的时间点
    // void HandleSprayFinish(int status);                                       // 处理喷印完成事件，status表示喷印结果状态

private:
    int length_enable_;          // 测长允许
    int waste_length_enable_;    // 长度判废
    float length_limit_max_;     // 判废管长止
    float length_limit_min_;     // 判废管长起
    float length_coupling_ = 0;  // 保护环长度
    int spray_length_precision_; // 喷涂长度小数位数(长度精度)
    bool m_bManualLength;

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

    // std::queue<int> m_flowNoQueue; // 保存3个已用过的流水号，用于流水号重复校验
    // // static const int MAX_QUEUE_SIZE = 10; // 队列最大大小

    // std::chrono::system_clock::time_point m_tEnterTime; // 管子进入工位时间，用于停机时间统计
    // // CBackBuffer m_backBuffer;                           // 后缓冲区对象

    // // class CBackBuffer *m_backBuffer;                    // 缓冲区指针
    // // std::mutex m_mutex;                                 // 线程安全锁
};