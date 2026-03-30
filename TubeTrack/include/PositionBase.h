#pragma once
#include "Tube.h"
#include <vector>
using namespace std;

class CPositionBase
{
public:
	CPositionBase();

private:
	vector<CTube> m_tubes;
	bool m_bOccupied;
	bool m_bTriggerEnabled;
	bool m_bUpdateTagEnabled;
	string convertToJson(const CTube &tube);

protected:
	bool m_bWbReleased; // 步进梁封锁状态位
	void GetDateTime(struct tm &t);
	void GetDateTimeString(string &dateStr, string &timeStr);

public:
	virtual bool Push(CTube &tube, int mode = 0); // 0根据信号自动，1异常情况下干预
	virtual bool Pop(CTube *pTube, int mode = 0);
	virtual bool Peek(CTube *pTube);
	virtual bool IsEmpty();
	virtual void Clear();
	virtual void Modify();
	virtual void RestoreFromTag();
	virtual void UpdateForm(); // 刷新画面
	virtual void EntryTriggerBeforePush(CTube &tube);
	virtual void EntryTrigger(const CTube &tube);
	virtual void ExitTrigger(const CTube &tube);
	virtual void SetOccupiedStatus(bool signal);				  // 根据接近开关信号设定工位有料无料状态
	virtual bool IsOccupied();									  // 返回接近开关传递的工位有料无料状态
	virtual void EnableTrigger();								  // 使能触发器
	virtual void DisableTrigger();								  // 禁用触发器
	virtual void EnableUpdateTag();								  // 使能TAG更新
	virtual void DisableUpdateTag();							  // 禁用TAG更新
	virtual bool UpdateTagIsOn() { return m_bUpdateTagEnabled; }; // 是否TAG
	virtual void ReleaseWB() { m_bWbReleased = true; };			  // 释放步进梁
	virtual void BlockWB() { m_bWbReleased = false; };			  // 封锁步进梁
	virtual bool WbReleased() { return m_bWbReleased; };		  // 返回本工位步进梁状态
	virtual void DebugOut();									  // 输出本工位物料信息
};
