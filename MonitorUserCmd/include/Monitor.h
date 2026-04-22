#pragma once

#include <string>

struct MonitorContext;

class CMonitor {
public:
	explicit CMonitor(MonitorContext& ctx);
	void Run();

private:
	MonitorContext& ctx_;

	void onMessage(const std::string& channel, const std::string& message);
	bool handleCommand(const std::string& message);
};
