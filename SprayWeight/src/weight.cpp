#include "weight.h"

#include <cmath>
#include <chrono>
#include <cstring>
#include <thread>

namespace {

constexpr int kWeightPollIntervalMs = 500;
constexpr int kWeightStabilizeMs = 2000;
constexpr int kWeightReadRetryCount = 5;
constexpr int kWeightReadRetryIntervalMs = 1000;
constexpr bool kUseMockWeight = true;
constexpr double kMockWeightKg = 0.0;
constexpr char kTaskQueue[] = "yjg4_TaskQ";
constexpr char kAlarmQueue[] = "yjg4_Alarm";
constexpr char kStartWeightingTag[] = "StartWeighting";

template <size_t N>
void copyString(char (&target)[N], const std::string& value) {
	std::memset(target, 0, N);
	if (N == 0) {
		return;
	}
	std::strncpy(target, value.c_str(), N - 1);
}

void postAlarm(int conn, const std::string& alarmQueue, const std::string& text, int grade = 9) {
	spdlog::warn("报警: {}", text);
	if (alarmQueue.empty()) {
		return;
	}

	AlarmEventWriteRecord alarm;
	alarm.type = 1;
	alarm.grade = grade;
	copyString(alarm.text, text);

	unsigned int error = 0;
	AlarmEventWriteRecord alarmCopy = alarm;
	if (!writeq(conn, alarmQueue.c_str(), &alarmCopy, sizeof(alarmCopy), &error)) {
		spdlog::error("写入队列失败: queue={}, error={}", alarmQueue, error);
	}
}

void logPlcWritePlaceholder(const std::string& pointName, const std::string& value, const std::string& reason) {
	spdlog::info("PLC写入占位: point={}, value={}, reason={}", pointName, value, reason);
}

}  // namespace

WeightWorker::WeightWorker(SprayWeightContext& ctx)
	: ctx_(ctx) {
}

void WeightWorker::Run() {
	spdlog::info("称重线程启动");

	unsigned int error = 0;
	bool startWeighting = false;
	if (!writeb(ctx_.gplatConn, kStartWeightingTag, &startWeighting, sizeof(startWeighting), &error)) {
		spdlog::error("写入标签失败: tag={}, error={}", kStartWeightingTag, error);
	}

	while (ctx_.running.load()) {
		startWeighting = false;
		error = 0;
		if (!readb(ctx_.gplatConn, kStartWeightingTag, &startWeighting, sizeof(startWeighting), &error)) {
			spdlog::warn("读取标签失败: tag={}, error={}", kStartWeightingTag, error);
			std::this_thread::sleep_for(std::chrono::milliseconds(kWeightPollIntervalMs));
			continue;
		}

		if (startWeighting) {
			startWeighting = false;
			error = 0;
			if (!writeb(ctx_.gplatConn, kStartWeightingTag, &startWeighting, sizeof(startWeighting), &error)) {
				spdlog::error("写入标签失败: tag={}, error={}", kStartWeightingTag, error);
			}

			// C# 原程序这里复位的是 qbd/gplat 进程同步标签，不是直接 PLC 点位。
			// 如果后续现场要求在此处直接驱动 PLC 启动秤，可补入如下伪代码：
			// 1. 建立 PLC 连接或复用已存在连接。
			// 2. 向“称重启动”输出位写 1，通知秤或采集模块开始一次采样。
			// 3. 等待 PLC 返回“称重完成/稳定”位，或者等待固定超时。
			// 4. 读取 PLC 内部缓存的重量值，或切换到串口读取原始重量帧。
			// 5. 完成后把 PLC 启动位复位为 0，避免信号常 1 锁死流程。
			logPlcWritePlaceholder(kStartWeightingTag, "0", "复位 gplat StartWeighting 同步标签；PLC 启动逻辑待接入");

			std::this_thread::sleep_for(std::chrono::milliseconds(kWeightStabilizeMs));
			const int weightValue = ReadWeightCentiKg();

			TaskRecord task;
			task.code = kTaskCodeWeightReady;
			task.n0 = weightValue;
			error = 0;
			TaskRecord taskCopy = task;
			if (!writeq(ctx_.gplatConn, kTaskQueue, &taskCopy, sizeof(taskCopy), &error)) {
				spdlog::error("写入队列失败: queue={}, error={}", kTaskQueue, error);
			}

			spdlog::info("称重完成: value={} (0.01kg)", weightValue);
		}

		std::this_thread::sleep_for(std::chrono::milliseconds(kWeightPollIntervalMs));
	}

	spdlog::info("称重线程退出");
}

int WeightWorker::ReadWeightCentiKg() const {
	// 串口驱动未接入前，先用伪代码占位，保留与 C# 相同的重试/稳定化流程。
	// 未来接入时可直接替换为：
	// 1. 打开串口，并在正式触发称重前先清空输入缓冲区，避免读到旧帧。
	// 2. 等待稳定延时后，循环读取原始字节流；每次读取都把十六进制内容打到日志，便于现场抓帧。
	// 3. 在缓冲区内查找 STX(0x02) 和 CR(0x0D)，并要求两者间距为 16 字节。
	// 4. 取 p1+4 开始的 6 位 ASCII 数字，得到秤返回的重量字符串。
	// 5. 检查第二个状态字 BIT3；若为 1，说明数据未稳定，继续下一轮重试。
	// 6. 若字符串可转数字，则按 C# 逻辑换算为 0.01kg: weight = (raw / 10 + 0.5)。
	// 7. 若重试超时返回 -2，若报文存在但数字解析失败返回 -1。
	for (int retry = 0; retry < kWeightReadRetryCount; ++retry) {
		if (kUseMockWeight) {
			if (kMockWeightKg < 0.0) {
				return -2;
			}
			return static_cast<int>(std::lround(kMockWeightKg * 100.0));
		}

		std::this_thread::sleep_for(std::chrono::milliseconds(kWeightReadRetryIntervalMs));
	}

	postAlarm(ctx_.gplatConn, kAlarmQueue, "称重串口驱动未接入，返回超时占位值");
	return -2;
}
