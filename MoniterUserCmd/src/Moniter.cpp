#include "Moniter.h"

#include <algorithm>
#include <chrono>
#include <csignal>
#include <sw/redis++/redis++.h>
#include "MoniterContext.h"
#include "../../include/logging.h"

extern volatile sig_atomic_t g_running;

namespace {

std::string Trim(const std::string& s)
{
	const auto begin = s.find_first_not_of(" \t\r\n");
	if (begin == std::string::npos) {
		return "";
	}

	const auto end = s.find_last_not_of(" \t\r\n");
	return s.substr(begin, end - begin + 1);
}

} // namespace

CMoniter::CMoniter(MoniterContext& ctx)
	: ctx_(ctx)
{
}

void CMoniter::Run()
{
	if (!ctx_.redis) {
		spdlog::error("Moniter启动失败: Redis连接未初始化");
		return;
	}

	const std::string channel = ctx_.targetChannel.empty() ? "optional_cmd" : ctx_.targetChannel;
	spdlog::info("Moniter开始监听Redis频道: {}", channel);

	auto sub = ctx_.redis->subscriber();

	try {
		sub.on_message([this](std::string recvChannel, std::string message) {
			this->onMessage(recvChannel, message);
		});

		sub.subscribe(channel);
	} catch (const std::exception& e) {
		spdlog::error("Redis订阅异常: {}", e.what());
		return;
	}

	while (g_running) {
		try {
			sub.consume();
		} catch (const sw::redis::TimeoutError&) {
			continue;
		} catch (const std::exception& e) {
			spdlog::error("Redis消费异常: {}", e.what());
			break;
		}
	}

	try {
		sub.unsubscribe(channel);
	} catch (const std::exception&) {
	}

	spdlog::info("Moniter工作线程退出");
}

void CMoniter::onMessage(const std::string& channel, const std::string& message)
{
	spdlog::info("收到命令消息, channel={}, payload={}", channel, message);

	std::string result;
	const bool ok = handleCommand(message, result);

	if (ok) {
		spdlog::info("命令处理成功: {}", result);
	} else {
		spdlog::warn("命令处理失败: {}", result);
	}
}

bool CMoniter::handleCommand(const std::string& message, std::string& result)
{
	const std::string input = Trim(message);
	if (input.empty()) {
		result = "ignored: empty payload";
		return false;
	}

	result = "ignored: no command processing";
	return true;
}

