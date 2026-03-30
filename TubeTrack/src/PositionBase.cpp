#include "PositionBase.h"
#include <iostream>
#include <sstream>
#include <iomanip>
#include <nlohmann/json.hpp>

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

bool CPositionBase::Push(CTube &tube, int /*mode*/)	//0根据信号自动，1异常情况下干预
{

	if (m_tubes.size() == 0)
	{
		if (m_bTriggerEnabled)
		{
			EntryTriggerBeforePush(tube);
		}
		m_tubes.push_back(tube);
		UpdateForm();
		if (m_bTriggerEnabled)
		{
			EntryTrigger(tube);
		}
		return true;
	}
	else
	{
		std::cout << "工位上已有管子，无法插入管子数据！" << std::endl;
		return false;
	}
}

bool CPositionBase::Pop(CTube *pTube, int /*mode*/)
{
	if (m_tubes.size() > 0)
	{
		*pTube = m_tubes[0];
		m_tubes.pop_back();
		UpdateForm();
		if (m_bTriggerEnabled)
		{
			ExitTrigger(*pTube);
		}	
		return true;
	}
	else
	{
		return false;
	}
}

bool CPositionBase::Peek(CTube *pTube)
{
	if (m_tubes.size() > 0)
	{
		*pTube = m_tubes[0];
		return true;
	}
	else
	{
		return false;
	}
}

bool CPositionBase::IsEmpty()
{
	return m_tubes.empty();
}

void CPositionBase::Clear()
{
	m_tubes.clear();
	UpdateForm();
}

void CPositionBase::Modify()
{}

void CPositionBase::RestoreFromTag()
{
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
    CTube tube;
    if (!Peek(&tube))
    {
        std::cout << "没有管子信息" << std::endl;
        return;
    }

    std::cout << "合同号    :" << tube.order_no << std::endl;
    std::cout << "项目号    :" << tube.item_no << std::endl;
    std::cout << "轧批号    :" << tube.roll_no << std::endl;
    std::cout << "炉号      :" << tube.melt_no << std::endl;
    std::cout << "试批号    :" << tube.lot_no << std::endl;
    std::cout << "管号      :" << tube.tube_no << std::endl;
    std::cout << "流水号    :" << tube.flow_no << std::endl;
    std::cout << "接箍批号  :" << tube.lotno_coupling << std::endl;
    std::cout << "接箍炉号  :" << tube.meltno_coupling << std::endl;

    // 数值格式化输出
    std::cout << std::fixed << std::setprecision(3);
    std::cout << "长度 (m)  :" << tube.length << std::endl;
    std::cout << "重量 (kg) :" << tube.weight << std::endl;
    std::cout << std::defaultfloat; // 恢复默认格式

	std::cout << "长度合格  :" << (tube.lengthOk ? "是" : "否") << std::endl;
	std::cout << "重量合格  :" << (tube.weightOk ? "是" : "否") << std::endl;
	std::cout << "是否喷印  :" << (tube.bSprayed ? "是" : "否") << std::endl;

    return;
}

string CPositionBase::convertToJson(const CTube & tube)
{

	// 使用nlohmann/json库实现生产计划转换为JSON格式字符串
    nlohmann::json j;
	j["calib_tube"] = tube.calib_tube;
    j["order_no"] = tube.order_no;
    j["item_no"] = tube.item_no;
    j["roll_no"] = tube.roll_no;
    j["melt_no"] = tube.melt_no;
    j["lot_no"] = tube.lot_no;
	j["tube_no"] = tube.tube_no;
	j["flow_no"] = tube.flow_no;
    j["lotno_coupling"] = tube.lotno_coupling;
    j["meltno_coupling"] = tube.meltno_coupling;
	j["length"] = tube.length;
	j["weight"] = tube.weight;
	j["lengthOk"] = tube.lengthOk;
	j["weightOk"] = tube.weightOk;
	j["bSprayed"] = tube.bSprayed;

    return j.dump(4);
}
