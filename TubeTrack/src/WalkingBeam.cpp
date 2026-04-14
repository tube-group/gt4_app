#include "WalkingBeam.h"
#include "logging.h"
#include "TubeTrackContext.h"
#include <chrono>
#include <thread>

WalkingBeam::WalkingBeam()
{
    // 初始化vector为6个元素，每个元素对应步进梁的六个工位
    m_tubes.resize(6);
    // 初始化所有工位为空
    for (int i = 0; i < 6; i++) {
        m_tubes[i] = nullptr;
    }
    
    m_bWbReleased = true; // 初始状态为释放状态
    m_bAtBase = true; // 初始状态在基座位置
}

bool WalkingBeam::Push(unique_ptr<CTube> tube0,
                       unique_ptr<CTube> tube1,
                       unique_ptr<CTube> tube2,
                       unique_ptr<CTube> tube3,
                       unique_ptr<CTube> tube4,
                       unique_ptr<CTube> tube5)
{
    spdlog::info("WalkingBeam: Starting push operation");
    
    // 将新管子放入对应工位
    m_tubes[0] = std::move(tube0);
    m_tubes[1] = std::move(tube1);
    m_tubes[2] = std::move(tube2);
    m_tubes[3] = std::move(tube3);
    m_tubes[4] = std::move(tube4);
    m_tubes[5] = std::move(tube5);
    
    spdlog::info("WalkingBeam: Push operation completed");
    // DebugOut();
    
    return true;
}

unique_ptr<CTube> WalkingBeam::Pop(int position)
{
    if (position < 0 || position >= 6) {
        spdlog::error("WalkingBeam: Invalid position {} for Pop", position);
        return nullptr;
    }
    
    auto tube = std::move(m_tubes[position]);
    m_tubes[position] = nullptr; // 弹出后工位置空
    
    return tube;
}

const CTube* WalkingBeam::GetTubeAt(int position) const
{
    if (position < 0 || position >= 6) {
        return nullptr;
    }
    return m_tubes[position].get();
}

bool WalkingBeam::IsEmpty() const
{
    for (const auto& tube : m_tubes) {
        if (tube) {
            return false; // 只要有一个工位有管子，就不是空的
        }
    }
    return true; // 所有工位都没有管子，步进梁为空
}

void WalkingBeam::Clear()
{
    spdlog::info("WalkingBeam: Clearing all positions");
    for (int i = 0; i < 6; i++) {
        m_tubes[i] = nullptr;
    }
}

void WalkingBeam::DebugOut()
{
    spdlog::info("=== WalkingBeam Status ===");
    spdlog::info("Released: {}", m_bWbReleased);
    
    for (int i = 0; i < 6; i++) {
        if (m_tubes[i]) {
            spdlog::info("Position {}: Tube {} (Order: {}, Length: {:.2f}m, Weight: {:.2f}kg)", 
                        i, 
                        m_tubes[i]->tube_no, 
                        m_tubes[i]->order_no,
                        m_tubes[i]->length,
                        m_tubes[i]->weight);
        } else {
            spdlog::info("Position {}: Empty", i);
        }
    }
    spdlog::info("========================");
}