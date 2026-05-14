#pragma once

struct TagPrintContext;

class TagPrint {
public:
	explicit TagPrint(TagPrintContext& ctx);

	// 线程主循环。
	// 按固定周期轮询喷码数据源，并在满足条件时执行一次喷码流程。
	void Run();
	void test();

private:

	// 共享运行时上下文，由 main() 创建并注入。
	TagPrintContext& ctx_;
};