#include "Moniter.h"

#include <algorithm>
#include <chrono>
#include <csignal>
#include <sw/redis++/redis++.h>
#include "MoniterContext.h"
#include "logging.h"
#include "usercmd.h"
#include "higplat.h"
#include <nlohmann/json.hpp>

using json = nlohmann::json;

extern volatile sig_atomic_t g_running;

namespace
{

	std::string Trim(const std::string &s)
	{
		const auto begin = s.find_first_not_of(" \t\r\n");
		if (begin == std::string::npos)
		{
			return "";
		}

		const auto end = s.find_last_not_of(" \t\r\n");
		return s.substr(begin, end - begin + 1);
	}

} // namespace

CMoniter::CMoniter(MoniterContext &ctx)
	: ctx_(ctx)
{
}

void CMoniter::Run()
{
	if (!ctx_.redis)
	{
		spdlog::error("Moniter启动失败: Redis连接未初始化");
		return;
	}

	const std::string channel = ctx_.targetChannel.empty() ? "optional_cmd" : ctx_.targetChannel;
	spdlog::info("Moniter开始监听Redis频道: {}", channel);

	auto sub = ctx_.redis->subscriber();

	try
	{
		sub.on_message([this](std::string recvChannel, std::string message)
					   { this->onMessage(recvChannel, message); });

		sub.subscribe(channel);
	}
	catch (const std::exception &e)
	{
		spdlog::error("Redis订阅异常: {}", e.what());
		return;
	}

	while (g_running)
	{
		try
		{
			sub.consume();
		}
		catch (const sw::redis::TimeoutError &)
		{
			continue;
		}
		catch (const std::exception &e)
		{
			spdlog::error("Redis消费异常: {}", e.what());
			break;
		}
	}

	try
	{
		sub.unsubscribe(channel);
	}
	catch (const std::exception &)
	{
	}

	spdlog::info("Moniter工作线程退出");
}

void CMoniter::onMessage(const std::string &channel, const std::string &message)
{
	spdlog::info("收到命令消息, channel={}, payload={}", channel, message);

	const bool ok = handleCommand(message);

	if (ok)
	{
		spdlog::info("命令处理成功: {}", message);
	}
	else
	{
		spdlog::warn("命令处理失败: {}", message);
	}
}

bool CMoniter::handleCommand(const std::string &message)
{
	json j = json::parse(message, nullptr, false);

	if (j.is_discarded())
	{
		spdlog::warn("无效的JSON命令: {}", message);
		return false;
	}

	if (!j.contains("cmd_name") || !j["cmd_name"].is_string())
	{
		spdlog::warn("命令缺少有效的cmd_name字段: {}", message);
		return false;
	}

	if (!j.contains("cmd_para") || !j["cmd_para"].is_string())
	{
		spdlog::warn("命令缺少有效的cmd_para字段: {}", message);
		return false;
	}

	spdlog::info("解析到命令: cmd_name={}, cmd_para={}", j["cmd_name"].get<std::string>(), j["cmd_para"].get<std::string>());

	json j_cmd_para = json::parse(j["cmd_para"].get<std::string>(), nullptr, false);
	if (j_cmd_para.is_discarded() || !j_cmd_para.is_object())
	{
		spdlog::warn("命令cmd_para不是有效的JSON对象: {}", j["cmd_para"].get<std::string>());
		return false;
	}

	try
	{
		if (j["cmd_name"] == "SetFeedNumCmd")
		{

			SetFeedNumCmd cmd;
			cmd.feed_num = std::stoi(j_cmd_para["feed_num"].get<std::string>());

			spdlog::info("处理SetFeedNumCmd命令: feed_num={}", cmd.feed_num);
		}
		else if (j["cmd_name"] == "MoveTubeCmd")
		{
			MoveTubeCmd cmd;

			cmd.from = j_cmd_para["from"].get<std::string>();
			cmd.to = j_cmd_para["to"].get<std::string>();

			spdlog::info("处理MoveTubeCmd命令: from={}, to={}", cmd.from.c_str(), cmd.to.c_str());

			unsigned int error;
			writeb(ctx_.gplatConn, "MOVE_TUBE_CMD", &cmd, sizeof(cmd), &error);
		}
		else
		{
			spdlog::warn("未知的命令类型: {}", j["cmd_name"].get<std::string>());
			return false;
		}
	}
	catch (const std::exception &e)
	{
		spdlog::warn("{}命令失败: {}", j["cmd_name"].get<std::string>(), e.what());
		return false;
	}

	return true;
}
