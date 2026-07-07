#include "Monitor.h"

#include <algorithm>
#include <chrono>
#include <csignal>
#include <stdexcept>
#include <sw/redis++/redis++.h>
#include "MonitorContext.h"
#include "logging.h"
#include "usercmd.h"
#include "higplat.h"
#include <nlohmann/json.hpp>

using json = nlohmann::json;

extern volatile sig_atomic_t g_running;

CMonitor::CMonitor(MonitorContext &ctx)
	: ctx_(ctx)
{
}

void CMonitor::Run()
{
	if (!ctx_.redis)
	{
		spdlog::error("Monitor启动失败: Redis连接未初始化");
		return;
	}

	const std::string channel = ctx_.targetChannel.empty() ? "optional_cmd" : ctx_.targetChannel;
	spdlog::info("Monitor开始监听Redis频道: {}", channel);

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

	spdlog::info("Monitor工作线程退出");
}

void CMonitor::onMessage(const std::string &channel, const std::string &message)
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

bool CMonitor::handleCommand(const std::string &message)
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

	const std::string cmdName = j["cmd_name"].get<std::string>();

	try
	{
		auto requireCmdPara = [&j, &cmdName]() -> const json &
		{
			if (!j.contains("cmd_para"))
			{
				throw std::runtime_error(cmdName + "命令缺少cmd_para字段");
			}

			return j["cmd_para"];
		};

		auto requireObjectCmdPara = [&requireCmdPara, &cmdName]() -> const json &
		{
			const auto &cmdPara = requireCmdPara();
			if (!cmdPara.is_object())
			{
				throw std::runtime_error(cmdName + "命令的cmd_para必须是对象");
			}

			return cmdPara;
		};

		if (cmdName == "SetFeedNumCmd")
		{
			const auto &cmdPara = requireObjectCmdPara();

			SetFeedNumCmd cmd;

			cmd.feed_num = cmdPara["feed_num"].get<int>();

			unsigned int error;
			writeb(ctx_.gplatConn, "SET_FEED_NUM_CMD", &cmd, sizeof(cmd), &error);

			spdlog::info("处理SetFeedNumCmd命令: feed_num={}", cmd.feed_num);

		}
		else if (cmdName == "MoveTubeCmd")
		{
			const auto &cmdPara = requireObjectCmdPara();

			MoveTubeCmd cmd;

			cmd.from = cmdPara["from"].get<std::string>();
			cmd.to = cmdPara["to"].get<std::string>();
			
			unsigned int error;
			writeb(ctx_.gplatConn, "MOVE_TUBE_CMD", &cmd, sizeof(cmd), &error);

			spdlog::info("处理MoveTubeCmd命令: from={}, to={}", cmd.from.c_str(), cmd.to.c_str());
		}
		else if (cmdName == "ModifyTubeCmd")
		{
			const auto &cmdPara = requireObjectCmdPara();

			ModifyTubeCmd cmd;

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
			cmd.length_ok = cmdPara["length_ok"].get<bool>();
			cmd.weight_ok = cmdPara["weight_ok"].get<bool>();
			cmd.lotno_coupling = cmdPara["lotno_coupling"].get<std::string>();
			cmd.meltno_coupling = cmdPara["meltno_coupling"].get<std::string>();

			unsigned int error;
			writeb(ctx_.gplatConn, "MODIFY_TUBE_CMD", &cmd, sizeof(cmd), &error);

			spdlog::info("处理ModifyTubeCmd命令: seq_no={}, position_name={}, order_no={}, item_no={}, roll_no={}, melt_no={}, lot_no={}, tube_no={}, flow_no={}, length={}, weight={}, length_ok={}, weight_ok={}, lotno_coupling={}, meltno_coupling={}",
						 cmd.seq_no, cmd.position_name.c_str(), cmd.order_no.c_str(), cmd.item_no.c_str(), cmd.roll_no.c_str(), cmd.melt_no.c_str(), cmd.lot_no.c_str(), cmd.tube_no, cmd.flow_no, cmd.length, cmd.weight, cmd.length_ok, cmd.weight_ok, cmd.lotno_coupling.c_str(), cmd.meltno_coupling.c_str());
		}
		else if (cmdName == "DeleteTubeCmd")
		{
			const auto &cmdPara = requireObjectCmdPara();

			DeleteTubeCmd cmd;

			cmd.seq_no = cmdPara["seq_no"].get<int>();
			cmd.position_name = cmdPara["position_name"].get<std::string>();

			unsigned int error;
			writeb(ctx_.gplatConn, "DELETE_TUBE_CMD", &cmd, sizeof(cmd), &error);

			spdlog::info("处理DeleteTubeCmd命令: seq_no={}, position_name={}", cmd.seq_no, cmd.position_name.c_str());
		}
		else if (cmdName == "SetCurrentContractCmd")
		{
			const auto &cmdPara = requireObjectCmdPara();

			SetCurrentContractCmd cmd;

			cmd.order_no = cmdPara["order_no"].get<std::string>();
			cmd.item_no = cmdPara["item_no"].get<std::string>();

			unsigned int error;
			writeb(ctx_.gplatConn, "SET_CURRENT_CONTRACT_CMD", &cmd, sizeof(cmd), &error);

			spdlog::info("处理SetCurrentContractCmd命令: order_no={}, item_no={}", cmd.order_no.c_str(), cmd.item_no.c_str());
		}
		else if (cmdName == "AddTubeCmd")
		{
			const auto &cmdPara = requireObjectCmdPara();

			AddTubeCmd cmd;

			cmd.seq_no = cmdPara["seq_no"].get<int>();
			cmd.position_name = cmdPara["position_name"].get<std::string>();

			unsigned int error;
			writeb(ctx_.gplatConn, "ADD_TUBE_CMD", &cmd, sizeof(cmd), &error);

			spdlog::info("处理AddTubeCmd命令: seq_no={}, position_name={}", cmd.seq_no, cmd.position_name.c_str());
		}
		else if (cmdName == "parameter_set_updated")
		{
			unsigned int error;
			int a = 0; // 这个命令没有参数，value可以是任意数据
			writeb(ctx_.gplatConn, "PARAMETER_SET_UPDATED", &a, sizeof(a), &error);
		}
		else if (cmdName == "start_weight_cmd")
		{
			unsigned int error;
			int value = 1;
			writeb(ctx_.gplatConn, "START_WEIGHT_EVENT", &value, sizeof(value), &error);
		}
		else if (cmdName == "stop_weight_cmd")
		{
			unsigned int error;
			int value = 0;
			writeb(ctx_.gplatConn, "START_WEIGHT_EVENT", &value, sizeof(value), &error);
		}
		else if (cmdName == "tag_print_event")
		{
			const auto &cmdPara = requireObjectCmdPara();

			unsigned int error;
			TagPrintEvent cmd; 
			cmd.order_no = cmdPara["order_no"].get<std::string>();
			cmd.item_no = cmdPara["item_no"].get<std::string>();	
			cmd.bundle_no = cmdPara["bundle_no"].get<std::string>();
			cmd.count = cmdPara["count"].get<int>();
			writeb(ctx_.gplatConn, "TAG_PRINT_EVENT", &cmd, sizeof(cmd), &error);
		}
		else if (cmdName == "l2_wb_release_cmd" || cmdName == "release_l2_wb_cmd")
		{
			const auto &cmdPara = requireCmdPara();

			unsigned int error;
			int value = 0;

			// cmdPara支持int/string/bool，0表示封锁步进梁，非0表示释放步进梁
			if (cmdPara.is_number_integer())
			{
				value = cmdPara.get<int>();
			}
			else if (cmdPara.is_string())
			{
				value = std::stoi(cmdPara.get<std::string>());
			}
			else if (cmdPara.is_boolean())
			{
				value = cmdPara.get<bool>() ? 1 : 0;
			}
			else
			{
				throw std::runtime_error("cmd_para类型非法, 期望int/string/bool");
			}

			bool boolValue = (value != 0);
			writeb(ctx_.gplatConn, "L2_WB_RELEASE", &boolValue, sizeof(boolValue), &error);
			spdlog::info("处理l2_wb_release_cmd命令: value={}", boolValue);

			ctx_.redis->set("L2_WB_RELEASE", boolValue ? "true" : "false");
			ctx_.redis->publish("RealDataChanged", "L2_WB_RELEASE");
		}
		else if (cmdName == "release_all_pos_cmd")
		{
			unsigned int error;
			int value = 1; // 释放所有工位
			writeb(ctx_.gplatConn, "RELEASE_ALL_POS_CMD", &value, sizeof(value), &error);
			spdlog::info("处理release_all_pos_cmd命令: value={}", value);
		}
		else if (cmdName == "bundle_cmd")
		{
			unsigned int error;
			int value = 1;
			writeb(ctx_.gplatConn, "BUNDLE_CMD", &value, sizeof(value), &error);

			spdlog::info("处理bundle_cmd命令: value={}", value);
		}
		else
		{
			spdlog::warn("未知的命令类型: {}", cmdName);
			return false;
		}
	}
	catch (const std::exception &e)
	{
		spdlog::warn("{}命令失败: {}", cmdName, e.what());
		return false;
	}

	return true;
}
