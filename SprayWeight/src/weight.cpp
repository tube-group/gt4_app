#include "weight.h"
#include "logging.h"
#include <iostream>

WeightWorker::WeightWorker(SprayWeightContext &ctx)
	: ctx_(ctx)
{
}

void WeightWorker::Run()
{
	spdlog::info("称重线程启动");

	unsigned int err = 0;
	if (!subscribe(ctx_.gplatConn, "START_WEIGHT_EVENT", &err))  //  尚未注册
	{
		spdlog::error("subscribe START_WEIGHT_EVENT failed, err={}", err);
		return;
	}
	if (!subscribe(ctx_.gplatConn, "timer_500ms", &err))
	{
		spdlog::error("subscribe timer_500ms failed, err={}", err);
		return;
	}

	while (ctx_.running.load())
	{

		char value[1024] = {0};
		std::string tagname;

		try
		{
			bool ret = waitpostdata(ctx_.gplatConn, tagname, value, 1024, -1, &err);
			if (!ret)
			{
				if (!ctx_.running.load())
				{
					break;
				}
				spdlog::warn("waitpostdata failed, err={}, reconnecting gPlat...", err);
				// 断线重连逻辑
				continue;
			}
		}
		catch (const std::exception &ex)
		{
			// 捕获 waitpostdata 过程中的异常，避免线程因单次异常直接退出。
			// 捕获异常后的处理逻辑，以打印异常内容到 cerr 为例
			std::cerr << "error: " << ex.what() << std::endl;
		}

		if (tagname == "timer_500ms")
		{
			continue;
		}

		if (tagname == "START_WEIGHT_EVENT")
		{
			try
			{

				double weightData = ReadWeightCentiKg();
				spdlog::info("称重事件触发，当前返回重量={} (0.01kg)", weightData);

				// 写入称重数据
				// write_plc_double(ctx_.gplatConn, "CURRENT_WEIGHT", weightData, &err);  // 发送当前重量到gplat
				// 写数据库
				// if (ctx_.pgConn && ctx_.pgConn->is_open()) {
				//     pqxx::work txn(*ctx_.pgConn);
				//     txn.exec0("INSERT INTO weight_log (weight) VALUES (" + txn.quote(weightData) + ")");
				//     txn.commit();
				// }
			}
			catch (const std::exception &ex)
			{
				// 捕获称重处理异常，避免单次失败导致线程退出。
				std::cerr << "error: " << ex.what() << std::endl;
			}
		}

	}
	spdlog::info("称重线程退出");
}

double WeightWorker::ReadWeightCentiKg() const
{
	double weight_data = -2.0;

	// 串口驱动未接入前，先返回占位失败值，保留与原 C# 流程一致的接入说明。
	// 后续接入时可按以下步骤恢复：
	// 1. 打开串口，并在正式触发称重前先清空输入缓冲区，避免读到旧帧。
	// 2. 等待稳定延时后，循环读取原始字节流；每次读取都把十六进制内容打到日志，便于现场抓帧。
	// 3. 在缓冲区内查找 STX(0x02) 和 CR(0x0D)，并要求两者间距为 16 字节。
	// 4. 取 p1+4 开始的 6 位 ASCII 数字，得到秤返回的重量字符串。
	// 5. 检查第二个状态字 BIT3；若为 1，说明数据未稳定，继续下一轮重试。
	// 6. 若字符串可转数字，则按 C# 逻辑换算为 0.01kg: weight = (raw / 10 + 0.5)。
	// 7. 若重试超时返回 -2，若报文存在但数字解析失败返回 -1。

	return weight_data;
}
