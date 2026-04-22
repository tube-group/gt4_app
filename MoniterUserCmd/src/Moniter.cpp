#include "Moniter.h"

#include <algorithm>
#include <chrono>
#include <csignal>
#include <stdexcept>
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

	if (!j.contains("cmd_para") || (!j["cmd_para"].is_object()))
	{
		spdlog::warn("命令缺少有效的cmd_para字段: {}", message);
		return false;
	}

	try
	{
		const auto &cmdPara = j["cmd_para"];

		if (j["cmd_name"] == "SetFeedNumCmd")
		{

			SetFeedNumCmd cmd;

			cmd.feed_num = cmdPara["feed_num"].get<int>();

			spdlog::info("处理SetFeedNumCmd命令: feed_num={}", cmd.feed_num);
		}
		else if (j["cmd_name"] == "MoveTubeCmd")
		{
			MoveTubeCmd cmd;

			cmd.from = cmdPara["from"].get<std::string>();
			cmd.to = cmdPara["to"].get<std::string>();

			spdlog::info("处理MoveTubeCmd命令: from={}, to={}", cmd.from.c_str(), cmd.to.c_str());
			// 	unsigned int error;
			// 	writeb(ctx_.gplatConn, "MOVE_TUBE_CMD", &cmd, sizeof(cmd), &error);
		}
		else if (j["cmd_name"] == "ModifyTubeCmd")
		{
			ModifyTubeCmd cmd;

			auto getModifyTubeBoolField = [&cmdPara](const char *primaryKey, const char *fallbackKey, bool defaultValue)
			{
				auto readBoolField = [&cmdPara](const char *key, bool &value)
				{
					const auto it = cmdPara.find(key);

					if (it == cmdPara.end() || it->is_null())
					{
						return false;
					}

					if (!it->is_boolean())
					{
						throw std::runtime_error(std::string(key) + "字段类型错误, 期望bool");
					}

					value = it->get<bool>();
					return true;
				};

				bool value = defaultValue;

				if (readBoolField(primaryKey, value))
				{
					return value;
				}

				if (fallbackKey != nullptr && readBoolField(fallbackKey, value))
				{
					return value;
				}

				return defaultValue;
			};

			cmd.seq_no = cmdPara["seq_no"].get<int>();
			cmd.position_name = cmdPara["position_name"].get<std::string>();
			cmd.order_no = cmdPara["order_no"].get<std::string>();
			cmd.item_no = cmdPara["item_no"].get<std::string>();
			cmd.roll_no = cmdPara["roll_no"].get<std::string>();
			cmd.melt_no = cmdPara["melt_no"].get<std::string>();
			cmd.lot_no = cmdPara["lot_no"].get<std::string>();
			cmd.tube_no = cmdPara["tube_no"].get<int>();
			cmd.flow_no = cmdPara["flow_no"].get<int>();
			cmd.length = cmdPara["length"].get<double>();
			cmd.weight = cmdPara["weight"].get<double>();
			cmd.lengthOk = getModifyTubeBoolField("lengthOk", "length_ok", false);
			cmd.weightOk = getModifyTubeBoolField("weightOk", "weight_ok", false);
			cmd.lotno_coupling = cmdPara["lotno_coupling"].get<std::string>();
			cmd.meltno_coupling = cmdPara["meltno_coupling"].get<std::string>();

			spdlog::info("处理ModifyTubeCmd命令: seq_no={}, position_name={}, order_no={}, item_no={}, roll_no={}, melt_no={}, lot_no={}, tube_no={}, flow_no={}, length={}, weight={}, lengthOk={}, weightOk={}, lotno_coupling={}, meltno_coupling={}",
						 cmd.seq_no, cmd.position_name.c_str(), cmd.order_no.c_str(), cmd.item_no.c_str(), cmd.roll_no.c_str(), cmd.melt_no.c_str(), cmd.lot_no.c_str(), cmd.tube_no, cmd.flow_no, cmd.length, cmd.weight, cmd.lengthOk, cmd.weightOk, cmd.lotno_coupling.c_str(), cmd.meltno_coupling.c_str());
		}
		else if (j["cmd_name"] == "DeleteTubeCmd")
		{
			DeleteTubeCmd cmd;

			cmd.seq_no = cmdPara["seq_no"].get<int>();
			cmd.position_name = cmdPara["position_name"].get<std::string>();

			spdlog::info("处理DeleteTubeCmd命令: seq_no={}, position_name={}", cmd.seq_no, cmd.position_name.c_str());
		}
		else if (j["cmd_name"] == "DeleteTubeCmd")
		{
			DeleteTubeCmd cmd;

			cmd.seq_no = cmdPara["seq_no"].get<int>();
			cmd.position_name = cmdPara["position_name"].get<std::string>();
			
			spdlog::info("处理DeleteTubeCmd命令: seq_no={}, position_name={}", cmd.seq_no, cmd.position_name.c_str());
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
