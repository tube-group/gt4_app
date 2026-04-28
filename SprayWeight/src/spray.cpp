#include "spray.h"
#include "usercmd.h"
#include <algorithm>
#include <cctype>
#include <ctime>
#include <exception>
#include <iomanip>
#include <regex>
#include <sstream>

namespace
{

	constexpr int kWaitPostBufferSize = 1024;
	constexpr int kWaitPostTimeoutMs = -1;

	static_assert(sizeof(StartSprayEvent) <= kWaitPostBufferSize, "StartSprayEvent exceeds waitpostdata buffer");

	std::string trimCopy(std::string value)
	{
		auto notSpace = [](unsigned char ch)
		{ return !std::isspace(ch); };
		value.erase(value.begin(), std::find_if(value.begin(), value.end(), notSpace));
		value.erase(std::find_if(value.rbegin(), value.rend(), notSpace).base(), value.end());
		return value;
	}

	std::string formatNumber(float value, int precision)
	{
		std::ostringstream stream;
		stream << std::fixed << std::setprecision(std::clamp(precision, 0, 3)) << value;
		return stream.str();
	}

	void replaceAll(std::string &text, const std::string &from, const std::string &to)
	{
		if (from.empty())
		{
			return;
		}

		std::size_t startPos = 0;
		while ((startPos = text.find(from, startPos)) != std::string::npos)
		{
			text.replace(startPos, from.size(), to);
			startPos += to.size();
		}
	}

	bool LoadSprayRuntimeConfig(SprayWeightContext &ctx, SprayJob &job)
	{

		if (ctx.pgConn == nullptr || !ctx.pgConn->is_open())
		{
			spdlog::warn("PostgreSQL 连接不可用，禁止喷印！");
			return false;
		}

		try
		{
			pqxx::nontransaction ntx(*ctx.pgConn);

			const pqxx::result orderResult = ntx.exec(
				"SELECT stencil_req_manual "
				"FROM api_order_data_t "
				"WHERE order_no = " +
				ntx.quote(job.orderNo) +
				" AND item_no = " + ntx.quote(job.itemNo) +
				" LIMIT 1");

			if (orderResult.empty())
			{
				spdlog::warn("未找到喷印模板: order_no={}, item_no={}", job.orderNo, job.itemNo);
				return false;
			}

			const auto &orderRow = orderResult[0];
			job.stencil_template = trimCopy(orderRow["stencil_req_manual"].as<std::string>(""));

			const pqxx::result parameterResult = ntx.exec(
				"SELECT spray_length_precision, spray_weight_precision, "
				"spray_length_type, spray_weight_type, spray_year_count "
				"FROM parameter_set "
				"LIMIT 1");

			if (parameterResult.empty())
			{
				spdlog::warn("parameter_set 表无数据，禁止喷印！");
				return false;
			}

			const auto &parameterRow = parameterResult[0];
			job.length_precision = parameterRow["spray_length_precision"].as<int>(0);
			job.weight_precision = parameterRow["spray_weight_precision"].as<int>(0);
			job.spray_length_type = parameterRow["spray_length_type"].as<int>(0);
			job.spray_weight_type = parameterRow["spray_weight_type"].as<int>(0);
			job.year_format = parameterRow["spray_year_count"].as<int>(0);
		}
		catch (const std::exception &e)
		{
			spdlog::error("读取喷印运行参数失败: order_no={}, item_no={}, error={}", job.orderNo, job.itemNo, e.what());
			return false;
		}

		job.stencil_template = std::regex_replace(job.stencil_template, std::regex("\\s+"), " ");
		if (job.stencil_template.empty())
		{
			spdlog::warn("喷印格式字符串为空，禁止喷印！");
			return false;
		}

		if (job.spray_length_type == 1)
		{
			if (std::regex_search(job.stencil_template, std::regex("长度 m", std::regex_constants::icase)))
			{
				spdlog::warn("喷印长度数值为英制，但单位是公制，禁止喷印！");
				return false;
			}
			job.length *= 3.280839f;
		}
		else if (std::regex_search(job.stencil_template, std::regex("长度 ft", std::regex_constants::icase)))
		{
			spdlog::warn("喷印长度数值为公制，但单位是英制，禁止喷印！");
			return false;
		}

		if (job.spray_weight_type == 1)
		{
			if (std::regex_search(job.stencil_template, std::regex("重量 kg", std::regex_constants::icase)))
			{
				spdlog::warn("喷印重量数值为英制，但单位是公制，禁止喷印！");
				return false;
			}
			job.weight *= 2.2046f;
			job.theory_weight *= 2.2046f;
		}
		else if (std::regex_search(job.stencil_template, std::regex("重量 lb", std::regex_constants::icase)))
		{
			spdlog::warn("喷印重量数值为公制，但单位是英制，禁止喷印！");
			return false;
		}

		return true;
	}

	std::string currentYearToken(int yearFormat, const std::string &sprayText)
	{
		const auto now = std::time(nullptr);
		const auto *localTime = std::localtime(&now);
		const int yearValue = localTime != nullptr ? (1900 + localTime->tm_year) : 1970;
		const std::string year = std::to_string(yearValue);

		if (yearFormat == 1)
		{
			return year.substr(2, 2);
		}

		if (yearFormat == 2)
		{
			return sprayText.find("季") != std::string::npos ? year.substr(3, 1) : year.substr(2, 2);
		}

		return year.substr(3, 1);
	}

	std::string currentQuarterToken()
	{
		const auto now = std::time(nullptr);
		const auto *localTime = std::localtime(&now);
		const int month = localTime != nullptr ? (localTime->tm_mon + 1) : 1;
		if (month <= 3)
		{
			return "1";
		}
		if (month <= 6)
		{
			return "2";
		}
		if (month <= 9)
		{
			return "3";
		}
		return "4";
	}

	std::string currentMonthToken()
	{
		const auto now = std::time(nullptr);
		const auto *localTime = std::localtime(&now);
		const int month = localTime != nullptr ? (localTime->tm_mon + 1) : 1;

		std::ostringstream stream;
		stream << std::setw(2) << std::setfill('0') << month;
		return stream.str();
	}

	std::string buildQrCodeContent(const SprayJob &job)
	{
		const auto now = std::time(nullptr);
		const auto *localTime = std::localtime(&now);
		const int yearValue = localTime != nullptr ? (1900 + localTime->tm_year) : 1970;
		const std::string year = std::to_string(yearValue);
		const std::string orderTail = job.orderNo.size() > 3 ? job.orderNo.substr(job.orderNo.size() - 3) : job.orderNo;

		std::string qrCode = "68";
		qrCode += year.substr(year.size() - 2);
		qrCode += orderTail;
		qrCode += std::to_string(job.flowNo);
		return qrCode;
	}

} // namespace

SprayWorker::SprayWorker(SprayWeightContext &ctx)
	: ctx_(ctx)
{
}

void SprayWorker::Run()
{
	spdlog::info("喷印线程启动");

	unsigned int err = 0;
	if (!subscribe(ctx_.gplatConn, "START_SPRAY_EVENT", &err))
	{
		spdlog::error("subscribe START_SPRAY_EVENT failed, err={}", err);
		return;
	}
	if (!subscribe(ctx_.gplatConn, "timer_500ms", &err))
	{
		spdlog::error("subscribe timer_500ms failed, err={}", err);
		return;
	}

	while (ctx_.running.load())
	{

		char value[kWaitPostBufferSize] = {0};
		std::string tagname;

		try
		{
			bool ret = waitpostdata(ctx_.gplatConn, tagname, value, kWaitPostBufferSize, kWaitPostTimeoutMs, &err);
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
			// 捕获异常后的处理逻辑，以打印异常内容到 cerr 为例
			std::cerr << "error: " << ex.what() << std::endl;
		}

		if (tagname == "timer_500ms")
		{
			continue;
		}

		if (tagname == "START_SPRAY_EVENT")
		{

			SprayJob job;
			StartSprayEvent sprayEvent = read_value<StartSprayEvent>(value);

			job.orderNo = sprayEvent.order_no.c_str();
			job.itemNo = sprayEvent.item_no.c_str();
			job.tubeNo = sprayEvent.tube_no;
			job.flowNo = sprayEvent.flow_no;
			job.meltNo = sprayEvent.melt_no.c_str();
			job.lotNo = sprayEvent.lot_no.c_str();
			job.length = sprayEvent.length;
			job.weight = sprayEvent.weight;
			job.theory_weight = sprayEvent.theory_weight;
			job.length_ok = sprayEvent.length_ok;
			job.weight_ok = sprayEvent.weight_ok;

			if (LoadSprayRuntimeConfig(ctx_, job))
			{
				SendSprayCommand(job);
			}
		}
	}

	spdlog::info("喷印线程退出");
}

void SprayWorker::test()
{

	StartSprayEvent sprayEvent;
	sprayEvent.order_no = "G2A2201255";
	sprayEvent.item_no = "0";
	sprayEvent.tube_no = 1;
	sprayEvent.flow_no = 1;
	sprayEvent.melt_no = "MELT001";
	sprayEvent.lot_no = "LOT001";
	sprayEvent.length = 12.345f;
	sprayEvent.weight = 67.89f;
	sprayEvent.theory_weight = 70.0f;
	sprayEvent.length_ok = true;
	sprayEvent.weight_ok = true;

	int gplatConn = connectgplat("127.0.0.1", 8777);

	if (gplatConn <= 0)
	{
		spdlog::error("gPlat连接失败");
		return;
	}

	std::this_thread::sleep_for(std::chrono::seconds(10));
	unsigned int err = 0;

	try
	{
		writeb(gplatConn, "START_SPRAY_EVENT", &sprayEvent, sizeof(sprayEvent), &err);
	}
	catch (const std::exception &ex)
	{
		// 捕获异常后的处理逻辑，以打印异常内容到 cerr 为例
		std::cerr << "error: " << ex.what() << std::endl;
	}

	// SprayJob job;
	// job.orderNo = sprayEvent.order_no.c_str();
	// job.itemNo = sprayEvent.item_no.c_str();
	// job.tubeNo = sprayEvent.tube_no;
	// job.flowNo = sprayEvent.flow_no;
	// job.meltNo = sprayEvent.melt_no.c_str();
	// job.lotNo = sprayEvent.lot_no.c_str();
	// job.length = sprayEvent.length;
	// job.weight = sprayEvent.weight;
	// job.theory_weight = sprayEvent.theory_weight;
	// job.length_ok = sprayEvent.length_ok;
	// job.weight_ok = sprayEvent.weight_ok;

	// if (LoadSprayRuntimeConfig(ctx_, job)) {
	// 	SendSprayCommand(job);
	// }
}

std::string SprayWorker::MakeSprayString(const SprayJob &job) const
{
	if (!job.weight_ok || !job.length_ok)
	{
		return " ";
	}

	std::string sprayText = job.stencil_template;
	replaceAll(sprayText, "理论重量", formatNumber(job.theory_weight, job.weight_precision));
	replaceAll(sprayText, "重量", formatNumber(job.weight, job.weight_precision));
	replaceAll(sprayText, "长度", formatNumber(job.length, job.length_precision));
	replaceAll(sprayText, "管号", std::to_string(job.tubeNo));
	replaceAll(sprayText, "流水号", std::to_string(job.flowNo));
	replaceAll(sprayText, "炉号", job.meltNo);
	replaceAll(sprayText, "试批号", job.lotNo);
	replaceAll(sprayText, "会标", "@HB1");
	replaceAll(sprayText, "TS标志", "@HB2");
	replaceAll(sprayText, "厂标", "@HB3");
	replaceAll(sprayText, "年", currentYearToken(job.year_format, sprayText));
	replaceAll(sprayText, "季", currentQuarterToken());
	replaceAll(sprayText, "月", currentMonthToken());

	const std::string qrCode = buildQrCodeContent(job);
	spdlog::info("二维码内容: {}", qrCode);
	// unsigned int err;
	// write_plc_string(ctx_.gplatConn, "BARCODE_STRING_TO_L1", qrCode, &err);  // 发送二维码喷印内容

	return sprayText;
}

void SprayWorker::SendSprayCommand(const SprayJob &job)
{

	std::string sprayText = " ";
	bool waste_flag;
	if (job.length_ok && job.weight_ok)
	{
		spdlog::info("发送废管标志位0");
		sprayText = MakeSprayString(job);
		waste_flag = false;
	}
	else
	{
		spdlog::warn("发送废管标志位1");
		waste_flag = true;
		// write_plc_string(ctx_.gplatConn, "BARCODE_STRING_TO_L1", "", &err);  // 发送二维码喷印内容
	}

	//  写PLC
	// unsigned int err;
	// write_plc_bool(ctx_.gplatConn, "SPRAY_WASTE_FLAG", waste_flag, &err);  // 废管标志位
	// write_plc_string(ctx_.gplatConn, "SPRAY_STRING_TO_L1", sprayText, &err);   // 发送喷印字符串
	// write_plc_bool(ctx_.gplatConn, "SPRAY_FINISH_NOUSE", true, &err);       // 喷印完成
	// write_plc_bool(ctx_.gplatConn, "QUICK_MARK_FINISH", true, &err);       // 条码喷印完成
	// write_plc_bool(ctx_.gplatConn, "SPRAY_FINISH", true, &err);       // 管体喷印完成
	// write_plc_bool(ctx_.gplatConn, "SPRAY_START_NOUSE", true, &err);        // 喷印发送标志位置位
	// std::this_thread::sleep_for(std::chrono::seconds(1));
	// write_plc_bool(ctx_.gplatConn, "SPRAY_START", true, &err);        // 启动管体喷印
	// write_plc_bool(ctx_.gplatConn, "QUICK_MARK_START", true, &err);        // 启动条码喷印

	// 用spdlog替代写PLC
	spdlog::info("写PLC: SPRAY_WASTE_FLAG={}, SPRAY_STRING_TO_L1={}, SPRAY_FINISH_NOUSE=true, QUICK_MARK_FINISH=true, SPRAY_FINISH=true, SPRAY_START_NOUSE=true, SPRAY_START=true, QUICK_MARK_START=true",
				 waste_flag ? 1 : 0,
				 sprayText);
}
