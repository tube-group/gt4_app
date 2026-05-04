#pragma once
#include "Tube.h"
#include "../../include/usercmd.h"
#include <deque>
#include <memory>
#include <cstddef>
using namespace std;

struct TubeTrackContext; // 前向声明

class CPositionBase
{
public:
	CPositionBase();

private:
	deque<unique_ptr<CTube>> m_tubes;
	bool m_bOccupied;
	bool m_bTriggerEnabled;
	bool m_bUpdateTagEnabled;

protected:
	TubeTrackContext* m_ctx = nullptr; // 上下文指针
	bool m_bWbReleased; // 步进梁封锁状态位
	void GetDateTime(struct tm &t);
	void GetDateTimeString(string &dateStr, string &timeStr);
	// string convertToJson(const CTube &tube);
	string convertToJson();

public:
	void SetContext(TubeTrackContext& ctx) { m_ctx = &ctx; }

public:
	virtual bool PushFront(unique_ptr<CTube> tube, int mode = 0);
	virtual bool PushBack(unique_ptr<CTube> tube, int mode = 0);
	virtual bool PushAt(unique_ptr<CTube> tube, int seqNo);
	virtual bool Push(unique_ptr<CTube> tube, int mode = 0); // 0根据信号自动，1异常情况下干预
	virtual unique_ptr<CTube> PopFront(int mode = 0);
	virtual unique_ptr<CTube> PopBack(int mode = 0);
	virtual unique_ptr<CTube> Pop(int mode = 0);
	virtual const CTube *Peek() const;
	virtual bool IsEmpty();
	virtual size_t Count() const;
	virtual void Clear();
	virtual bool Modify(const ModifyTubeCmd &cmd);
	virtual bool Delete(int seqNo);
	virtual void RestoreFromTag();
	virtual bool RestoreFromJson(const string &jsonStr, const char *sourceName = nullptr);
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
