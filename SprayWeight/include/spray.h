#pragma once

#include <string>
#include "SprayWeightContext.h"

struct SprayJob {
	std::string orderNo;  // 合同号
	std::string itemNo;  // 项目号
	int tubeNo = 0;  // 管号
	int flowNo = 0;  // 流水号
	std::string meltNo;  // 炉号
	std::string lotNo;  // 试批号
	double length = 0.0;
	double weight = 0.0;
	double theory_weight = 0.0;
	int spray_length_type = 0; // 长度单位类型，0=公制，1=英制
	int spray_weight_type = 0; // 重量单位类型，0=公制，1=英制
	bool length_ok = false;
	bool weight_ok = false;
	int length_precision = 0;  // 长度小数位精度
	int weight_precision = 0;  // 重量小数位精度
	int year_format = 0;
	std::string stencil_template;  // 喷印要求
};

// 喷码工作线程。
// 负责选择待喷钢管、生成喷码文本，并将喷码命令发送到下游设备或接口。
class SprayWorker {
public:
	explicit SprayWorker(SprayWeightContext& ctx);

	// 线程主循环。
	// 按固定周期轮询喷码数据源，并在满足条件时执行一次喷码流程。
	void Run();
	void test();

private:
	// 根据任务数据拼接最终的喷码文本。
	std::string MakeSprayString(const SprayJob& job) const;
	// 将喷码文本发送给实际执行端。
	// 当前实现输出 PLC 占位日志。
	void SendSprayCommand(const SprayJob& job);

	// 共享运行时上下文，由 main() 创建并注入。
	SprayWeightContext& ctx_;
};