#include "PositionBase.h"
#include <iostream>
#include <sstream>
#include <iomanip>
#include <utility>
#include <nlohmann/json.hpp>
#include "../../include/logging.h"   // spdlog

CPositionBase::CPositionBase()
{
	m_bTriggerEnabled	= true;
	m_bUpdateTagEnabled = true;
	m_bWbReleased = true;
}

void CPositionBase::GetDateTime(struct tm & t)
{
	time_t now;             //声明time_t类型变量
	time(&now);				//获取系统日期和时间
	localtime_r(&now, &t);  //获取当地日期和时间
}

void CPositionBase::GetDateTimeString(string & dateStr, string & timeStr)
{
	struct tm t;			//tm结构指针
	time_t now;             //声明time_t类型变量
	time(&now);				//获取系统日期和时间
	localtime_r(&now, &t);  //获取当地日期和时间

	// 输出 tm 结构的各个组成部分
	std::stringstream ss1, ss2;
	ss1 << 1900 + t.tm_year << setw(2) << setfill('0') << 1 + t.tm_mon << setw(2) << setfill('0') << t.tm_mday;
	dateStr = ss1.str();
	ss2 << setw(2) << setfill('0') << t.tm_hour << setw(2) << setfill('0') << t.tm_min << setw(2) << setfill('0') << t.tm_sec;
	timeStr = ss2.str();
}

bool CPositionBase::PushFront(unique_ptr<CTube> tube, int /*mode*/)
{
	if (!tube)
	{
		return false;
	}

	if (m_bTriggerEnabled)
	{
		EntryTriggerBeforePush(*tube);
	}
	CTube *tubePtr = tube.get();
	m_tubes.push_front(std::move(tube));
	UpdateForm();
	if (m_bTriggerEnabled)
	{
		EntryTrigger(*tubePtr);
	}
	return true;
}

bool CPositionBase::PushBack(unique_ptr<CTube> tube, int /*mode*/)
{
	if (!tube)
	{
		return false;
	}

	if (m_bTriggerEnabled)
	{
		EntryTriggerBeforePush(*tube);
	}
	CTube *tubePtr = tube.get();
	m_tubes.push_back(std::move(tube));
	UpdateForm();
	if (m_bTriggerEnabled)
	{
		EntryTrigger(*tubePtr);
	}
	return true;
}

bool CPositionBase::PushAt(unique_ptr<CTube> tube, int seqNo)
{
	// seqNo >= 0 时按头部正向计数；seqNo < 0 时按尾部插入位反向计数。
	// 例如：0 表示第一个位置，1 表示第二个位置；-1 表示末尾，-2 表示末尾前一个位置。

	if (!tube)
	{
		return false;
	}

	// 计算插入位置的索引
	ptrdiff_t insertIndex = 0;// 使用有符号整数，支持负数计算结果
	if (seqNo >= 0)
	{
		// 正向插入：0→第一个位置，1→第二个位置，2→第三个位置...
		insertIndex = static_cast<ptrdiff_t>(seqNo);
	}
	else
	{
		// 反向插入：-1→末尾，-2→倒数第二个，-3→倒数第三个...
		// 转换公式：实际位置 = 当前大小 + 负索引 + 1
		insertIndex = static_cast<ptrdiff_t>(m_tubes.size()) + static_cast<ptrdiff_t>(seqNo) + 1;
	}

	// 检查插入位置是否合法,insertIndex 必须在 [0, size()] 范围内
	if (insertIndex < 0 || insertIndex > static_cast<ptrdiff_t>(m_tubes.size()))
	{
		return false;
	}

	//末尾插入直接调用 PushBack函数
	if (insertIndex == static_cast<ptrdiff_t>(m_tubes.size()))
	{
		return PushBack(std::move(tube));
	}

	if (m_bTriggerEnabled)
	{
		EntryTriggerBeforePush(*tube);
	}

	// 在指定位置插入管子
	CTube *tubePtr = tube.get();
	auto it = m_tubes.begin() + insertIndex;
	m_tubes.insert(it, std::move(tube));
	UpdateForm();

	if (m_bTriggerEnabled)
	{
		EntryTrigger(*tubePtr);
	}

	return true;
}

//默认从后端插入管子
bool CPositionBase::Push(unique_ptr<CTube> tube, int mode)
{
	return PushBack(std::move(tube), mode);
}

unique_ptr<CTube> CPositionBase::PopFront(int /*mode*/)
{
	if (m_tubes.size() > 0)
	{
		auto tube = std::move(m_tubes.front());
		m_tubes.pop_front();
		UpdateForm();
		if (m_bTriggerEnabled && tube)
		{
			ExitTrigger(*tube);
		}	
		return tube;
	}
	else
	{
		return nullptr;
	}
}

unique_ptr<CTube> CPositionBase::PopBack(int /*mode*/)
{
	if (m_tubes.size() > 0)
	{
		auto tube = std::move(m_tubes.back());
		m_tubes.pop_back();
		UpdateForm();
		if (m_bTriggerEnabled && tube)
		{
			ExitTrigger(*tube);
		}	
		return tube;
	}
	else
	{
		return nullptr;
	}
}

//默认从前端弹出管子
unique_ptr<CTube> CPositionBase::Pop(int /*mode*/)
{
	return PopFront();
}

const CTube *CPositionBase::Peek() const
{
	if (m_tubes.size() > 0)
	{
		return m_tubes.front().get();
	}
	else
	{
		return nullptr;
	}
}

bool CPositionBase::IsEmpty()
{
	return m_tubes.empty();
}

size_t CPositionBase::Count() const
{
	return m_tubes.size();
}

void CPositionBase::Clear()
{
	m_tubes.clear();
	UpdateForm();
}

bool CPositionBase::Modify(const ModifyTubeCmd &cmd)
{
	if (cmd.seq_no < 0)
	{
		return false;
	}

	size_t index = static_cast<size_t>(cmd.seq_no);
	if (index >= m_tubes.size())
	{
		return false;
	}

	auto &tube = m_tubes[index];
	if (!tube)
	{
		return false;
	}

	tube->order_no = cmd.order_no.c_str();
	tube->item_no = cmd.item_no.c_str();
	tube->roll_no = cmd.roll_no.c_str();
	tube->melt_no = cmd.melt_no.c_str();
	tube->lot_no = cmd.lot_no.c_str();
	tube->tube_no = cmd.tube_no;
	tube->flow_no = cmd.flow_no;
	tube->length = static_cast<float>(cmd.length);
	tube->weight = static_cast<float>(cmd.weight);
	tube->length_ok = cmd.length_ok;
	tube->weight_ok = cmd.weight_ok;
	tube->lotno_coupling = cmd.lotno_coupling.c_str();
	tube->meltno_coupling = cmd.meltno_coupling.c_str();
	UpdateForm();
	return true;
}

bool CPositionBase::Delete(int seqNo)
{
	if (seqNo < -1)
	{
		return false;
	}
	//如果seqNo=-1,则清空该工位队列所有管子
	if (seqNo == -1)
	{
		//调用Clear()函数清空管子队列并更新画面
		Clear();
		return true;
	}

	size_t index = static_cast<size_t>(seqNo);
	if (index >= m_tubes.size())
	{
		return false;
	}

	auto it = m_tubes.begin() + static_cast<ptrdiff_t>(index);
	auto tube = std::move(*it);
	m_tubes.erase(it);
	UpdateForm();
	if (m_bTriggerEnabled && tube)
	{
		ExitTrigger(*tube);
	}
	return true;
}

void CPositionBase::RestoreFromTag()
{
}

bool CPositionBase::RestoreFromJson(const string &jsonStr, const char *sourceName)
{
	try
	{
		// 情况1：redis数据为空(首次启动)
		if (jsonStr.empty())
		{
			spdlog::info("{} 首次启动，初始化为空工位", sourceName ? sourceName : "Position");
			m_tubes.clear();
			return true; // 正常状态
		}

		// 情况2：有数据但为空JSON对象
		nlohmann::json j = nlohmann::json::parse(jsonStr);

		if (!m_tubes.empty())
		{
			spdlog::warn("{} 非空状态下执行恢复，将覆盖现有数据", sourceName);
			m_tubes.clear();
		}
		// 处理空对象 {} 和空数组 [] 两种情况
		if (j.is_object() && j.empty())
		{
			spdlog::info("{} 从Redis恢复为空工位", sourceName ? sourceName : "Position");
			return true;
		}
		// 情况3：数据格式错误
		if (!j.is_array())
		{
			spdlog::warn("{} Redis数据格式不是数组，跳过恢复: {}", sourceName ? sourceName : "Position", jsonStr);
			return false;
		}
		// 情况4：正常恢复数据
		for (const auto &tubeJson : j)
		{
			auto tube = std::make_unique<CTube>();
			tube->order_no = tubeJson.value("order_no", "");
			tube->item_no = tubeJson.value("item_no", "");
			tube->roll_no = tubeJson.value("roll_no", "");
			tube->melt_no = tubeJson.value("melt_no", "");
			tube->lot_no = tubeJson.value("lot_no", "");
			tube->tube_no = tubeJson.value("tube_no", 0);
			tube->flow_no = tubeJson.value("flow_no", 0);
			tube->lotno_coupling = tubeJson.value("lotno_coupling", "");
			tube->meltno_coupling = tubeJson.value("meltno_coupling", "");
			tube->length = tubeJson.value("length", 0.0f);
			tube->weight = tubeJson.value("weight", 0.0f);
			tube->length_ok = tubeJson.value("length_ok", true);
			tube->weight_ok = tubeJson.value("weight_ok", true);
			tube->sprayed = tubeJson.value("sprayed", false);
			m_tubes.push_back(std::move(tube));
		}

		spdlog::info("{} 从Redis恢复了 {} 根管子", sourceName ? sourceName : "Position", m_tubes.size());
		return true;
	}
	catch (const std::exception &e)
	{
		spdlog::error("{} Redis恢复失败: {}", sourceName ? sourceName : "Position", e.what());
		return false;
	}
}

void CPositionBase::UpdateForm()
{
}

void CPositionBase::EntryTriggerBeforePush(CTube & /*tube*/)
{
}

void CPositionBase::EntryTrigger(const CTube & /*tube*/)
{
}

void CPositionBase::ExitTrigger(const CTube & /*tube*/)
{
}

void CPositionBase::SetOccupiedStatus(bool signal)
{
	m_bOccupied = signal;
}

bool CPositionBase::IsOccupied()
{
	return m_bOccupied;
}

void CPositionBase::EnableTrigger()
{
	m_bTriggerEnabled = true;
}

void CPositionBase::DisableTrigger()
{
	m_bTriggerEnabled = false;
}

void CPositionBase::EnableUpdateTag()
{
	m_bUpdateTagEnabled = true;
}

void CPositionBase::DisableUpdateTag()
{
	m_bUpdateTagEnabled = false;
}

void CPositionBase::DebugOut()
{
	const CTube *tube = Peek();
	if (!tube)
    {
        std::cout << "没有管子信息" << std::endl;
        return;
    }

    // std::cout << "合同号    :" << tube.order_no << std::endl;
    // std::cout << "项目号    :" << tube.item_no << std::endl;
    // std::cout << "轧批号    :" << tube.roll_no << std::endl;
    // std::cout << "炉号      :" << tube.melt_no << std::endl;
    // std::cout << "试批号    :" << tube.lot_no << std::endl;
    // std::cout << "管号      :" << tube.tube_no << std::endl;
    // std::cout << "流水号    :" << tube.flow_no << std::endl;
    // std::cout << "接箍批号  :" << tube.lotno_coupling << std::endl;
    // std::cout << "接箍炉号  :" << tube.meltno_coupling << std::endl;
	spdlog::info("合同号    : {}", tube->order_no);
	spdlog::info("项目号    : {}", tube->item_no);
	spdlog::info("轧批号    : {}", tube->roll_no);
	spdlog::info("炉号      : {}", tube->melt_no);
	spdlog::info("试批号    : {}", tube->lot_no);
	spdlog::info("管号      : {}", tube->tube_no);
	spdlog::info("流水号    : {}", tube->flow_no);
	spdlog::info("接箍批号  : {}", tube->lotno_coupling);
	spdlog::info("接箍炉号  : {}", tube->meltno_coupling);


    // 数值格式化输出
    // std::cout << std::fixed << std::setprecision(3);
    // std::cout << "长度 (m)  :" << tube.length << std::endl;
    // std::cout << "重量 (kg) :" << tube.weight << std::endl;
    // std::cout << std::defaultfloat; // 恢复默认格式
	spdlog::info("长度 (m)  : {}", tube->length);
	spdlog::info("重量 (kg) : {}", tube->weight);


	// std::cout << "长度合格  :" << (tube.lengthOk ? "是" : "否") << std::endl;
	// std::cout << "重量合格  :" << (tube.weightOk ? "是" : "否") << std::endl;
	// std::cout << "是否喷印  :" << (tube.bSprayed ? "是" : "否") << std::endl;
	spdlog::info("长度合格  : {}", tube->length_ok ? "是" : "否");
	spdlog::info("重量合格  : {}", tube->weight_ok ? "是" : "否");
	spdlog::info("是否喷印  : {}", tube->sprayed ? "是" : "否");

    return;
}

// string CPositionBase::convertToJson(const CTube & tube)
// {
// 	// 使用nlohmann/json库实现生产计划转换为JSON格式字符串
//     nlohmann::json j;
//     j["order_no"] = tube.order_no;
//     j["item_no"] = tube.item_no;
//     j["roll_no"] = tube.roll_no;
//     j["melt_no"] = tube.melt_no;
//     j["lot_no"] = tube.lot_no;
// 	j["tube_no"] = tube.tube_no;
// 	j["flow_no"] = tube.flow_no;
//     j["lotno_coupling"] = tube.lotno_coupling;
//     j["meltno_coupling"] = tube.meltno_coupling;
// 	j["length"] = tube.length;
// 	j["weight"] = tube.weight;
// 	j["lengthOk"] = tube.lengthOk;
// 	j["weightOk"] = tube.weightOk;
// 	j["bSprayed"] = tube.bSprayed;

//     return j.dump(4);
// }

//直接获取当前工位管子信息并转换为JSON字符串
string CPositionBase::convertToJson()
{
	if (m_tubes.empty())
	{
		return nlohmann::json::array().dump(); // 统一空工位为空数组字符串，避免运行时类型转换异常
	}

	// 枚举当前工位的所有管子并转换为JSON数组
	nlohmann::json j = nlohmann::json::array();
	for (const auto& tubePtr : m_tubes)
	{
		if (tubePtr)
		{
			nlohmann::json tubeJson;
			tubeJson["order_no"] = tubePtr->order_no;
			tubeJson["item_no"] = tubePtr->item_no;
			tubeJson["roll_no"] = tubePtr->roll_no;
			tubeJson["melt_no"] = tubePtr->melt_no;
			tubeJson["lot_no"] = tubePtr->lot_no;
			tubeJson["tube_no"] = tubePtr->tube_no;
			tubeJson["flow_no"] = tubePtr->flow_no;
			tubeJson["lotno_coupling"] = tubePtr->lotno_coupling;
			tubeJson["meltno_coupling"] = tubePtr->meltno_coupling;
			tubeJson["length"] = tubePtr->length;
			tubeJson["weight"] = tubePtr->weight;
			tubeJson["length_ok"] = tubePtr->length_ok;
			tubeJson["weight_ok"] = tubePtr->weight_ok;
			tubeJson["sprayed"] = tubePtr->sprayed;

			j.push_back(tubeJson);
		}
	}

	return j.dump(4);
}
