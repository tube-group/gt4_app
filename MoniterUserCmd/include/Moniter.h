#pragma once

#include <string>

struct MoniterContext;

class CMoniter {
public:
	explicit CMoniter(MoniterContext& ctx);
	void Run();

private:
	MoniterContext& ctx_;

	void onMessage(const std::string& channel, const std::string& message);
	bool handleCommand(const std::string& message, std::string& result);
};
