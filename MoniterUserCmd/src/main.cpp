#include "RedisCommandSubscriber.h"
#include "iniconfig.h"
#include "logging.h"

#include <csignal>
#include <exception>
#include <filesystem>
#include <string>
#include <unordered_set>
#include <vector>

namespace {

RedisCommandSubscriber* g_subscriber = nullptr;

void HandleSignal(int signalNumber) {
	spdlog::info("[OperationCmdListener] 收到退出信号: {}", signalNumber);
	if (g_subscriber != nullptr) {
		g_subscriber->Stop();
	}
}

void RegisterSignalHandlers() {
	std::signal(SIGINT, HandleSignal);
	std::signal(SIGTERM, HandleSignal);
}

bool LoadRedisConfigFromIni(const std::string& iniPath, RedisSubscriberConfig& cfg) {
	auto& config = CConfig::GetInstance();
	if (!config.Load(iniPath)) {
		return false;
	}

	cfg.host = config.GetStringDefault("redis_host", cfg.host);
	cfg.port = config.GetIntDefault("redis_port", cfg.port);
	cfg.password = config.GetStringDefault("redis_password", cfg.password);
	cfg.channel = config.GetStringDefault("redis_channel", cfg.channel);
	cfg.socketTimeoutMs =
		config.GetIntDefault("redis_socket_timeout_ms", cfg.socketTimeoutMs);
	cfg.reconnectDelayMs =
		config.GetIntDefault("redis_reconnect_delay_ms", cfg.reconnectDelayMs);

	return true;
}

bool LoadRedisConfigWithAutoPath(const char* argv0, RedisSubscriberConfig& cfg) {
	namespace fs = std::filesystem;

	std::vector<fs::path> candidates;

	if (argv0 != nullptr && argv0[0] != '\0') {
		const fs::path exePath = fs::absolute(fs::path(argv0));
		const fs::path exeDir = exePath.parent_path();
		candidates.emplace_back(exeDir / "config" / "tubetrack.ini");
		candidates.emplace_back(exeDir / ".." / "config" / "tubetrack.ini");
		candidates.emplace_back(exeDir / ".." / ".." / "config" / "tubetrack.ini");
	}

	const fs::path cwd = fs::current_path();
	candidates.emplace_back(cwd / "config" / "tubetrack.ini");
	candidates.emplace_back(cwd / ".." / "config" / "tubetrack.ini");
	candidates.emplace_back(cwd / ".." / ".." / "config" / "tubetrack.ini");

	std::unordered_set<std::string> visited;
	std::vector<std::string> tried;

	for (const auto& candidate : candidates) {
		const fs::path normalized = fs::absolute(candidate).lexically_normal();
		const std::string pathString = normalized.string();

		if (visited.find(pathString) != visited.end()) {
			continue;
		}
		visited.insert(pathString);
		tried.push_back(pathString);

		if (!fs::exists(normalized)) {
			continue;
		}

		if (LoadRedisConfigFromIni(pathString, cfg)) {
			spdlog::info("[OperationCmdListener] 已加载配置文件: {}", pathString);
			return true;
		}
	}

	spdlog::error("[OperationCmdListener] 加载配置文件失败，当前工作目录: {}", cwd.string());
	for (const auto& path : tried) {
		spdlog::error("[OperationCmdListener] 已尝试路径: {}", path);
	}

	return false;
}

}  // namespace

int main(int argc, char* argv[]) {
	LogConfig logConfig;
	logConfig.logger_name = "moniter_user_cmd";
	logConfig.log_console = true;
	logConfig.level = "info";
	logConfig.filename = "logs/MoniterUserCmd.log";

	if (!initLogging(logConfig)) {
		return 1;
	}

	RedisSubscriberConfig config;
	if (!LoadRedisConfigWithAutoPath(argv[0], config)) {
		shutdownLogging();
		return 1;
	}

	try {
		// 命令行参数仅作为覆盖项，优先级高于 ini 配置。
		if (argc > 1) {
			config.host = argv[1];
		}
		if (argc > 2) {
			config.port = std::stoi(argv[2]);
		}
		if (argc > 3) {
			config.password = argv[3];
		}
		if (argc > 4) {
			config.channel = argv[4];
		}
	} catch (const std::exception& error) {
		spdlog::error("[OperationCmdListener] 启动参数错误: {}", error.what());
		spdlog::error("用法: MoniterUserCmd [host] [port] [password] [channel]");
		shutdownLogging();
		return 1;
	}

	spdlog::info("[OperationCmdListener] Redis={}:{}, channel={}",
					 config.host,
					 config.port,
					 config.channel);

	RedisCommandSubscriber subscriber(config);
	g_subscriber = &subscriber;

	RegisterSignalHandlers();

	spdlog::info("[OperationCmdListener] 启动监听器，按 Ctrl+C 退出");
	subscriber.Start();
	spdlog::info("[OperationCmdListener] 监听器已停止");

	g_subscriber = nullptr;
	shutdownLogging();
	return 0;
}
