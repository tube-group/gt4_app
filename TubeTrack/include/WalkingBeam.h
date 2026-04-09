#pragma once
#include "Tube.h"
#include <vector>
#include <memory>
using namespace std;

struct TubeTrackContext; // 前向声明

class WalkingBeam
{
public:
    WalkingBeam();
    ~WalkingBeam() = default;

private:
    vector<unique_ptr<CTube>> m_tubes;  // 6个工位的管子存储
    TubeTrackContext* m_ctx = nullptr;  // 上下文指针
    bool m_bWbReleased;                 // 步进梁封锁状态位

public:
    // 设置上下文
    void SetContext(TubeTrackContext& ctx) { m_ctx = &ctx; }
    
    // 步进梁推进操作：接受6个管子，执行步进梁动作
    bool Push(unique_ptr<CTube> tube0,
              unique_ptr<CTube> tube1,
              unique_ptr<CTube> tube2,
              unique_ptr<CTube> tube3,
              unique_ptr<CTube> tube4,
              unique_ptr<CTube> tube5);
    
    // 获取指定工位的管子（只读访问）
    const CTube* GetTubeAt(int position) const;
    
    // 清空所有工位
    void Clear();
    
    // 获取步进梁状态
    bool IsReleased() const { return m_bWbReleased; }
    
    // 设置步进梁状态
    void SetReleased(bool released) { m_bWbReleased = released; }
    
    // 调试输出
    void DebugOut();
};