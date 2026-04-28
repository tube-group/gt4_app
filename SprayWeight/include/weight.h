#pragma once

#include "SprayWeightContext.h"

// 称重工作线程。
// 负责等待称重触发、读取重量值，并将结果写回任务队列或相关标签。
class WeightWorker {
public:
	explicit WeightWorker(SprayWeightContext& ctx);

	// 线程主循环。
	// 按固定重试、稳定等待和上报策略执行称重流程。
	void Run();

private:
	// 从称重设备读取当前重量，返回值单位为 0.01kg。
	// 使用整数可以减少串口解析和队列传输过程中的浮点误差。
	int ReadWeightCentiKg() const;

	// 共享运行时上下文，由 main() 创建。
	SprayWeightContext& ctx_;
};
