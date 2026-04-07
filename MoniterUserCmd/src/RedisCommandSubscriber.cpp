#include "RedisCommandSubscriber.h"

#include "logging.h"

#include <chrono>
#include <stdexcept>
#include <thread>
#include <utility>

namespace {

void DefaultCommandHandler(const OperationCommand& command) {
  spdlog::info("[OperationCmdListener] channel={}, cmd={}, payload={}",
               command.channel,
               command.commandName,
               command.rawPayload);
}

}  // namespace

RedisCommandSubscriber::RedisCommandSubscriber(RedisSubscriberConfig config)
    : config_(std::move(config)), commandHandler_(DefaultCommandHandler) {}

void RedisCommandSubscriber::SetCommandHandler(CommandHandler handler) {
  if (handler) {
    commandHandler_ = std::move(handler);
  }
}

void RedisCommandSubscriber::Start() {
  if (running_.exchange(true)) {
    spdlog::warn("[OperationCmdListener] 监听器已在运行");
    return;
  }

  while (running_.load()) {
    try {
      Connect();
      ConsumeLoop();
    } catch (const sw::redis::Error& error) {
      if (!running_.load()) {
        break;
      }

      spdlog::error("[OperationCmdListener] Redis 错误: {}", error.what());
      std::this_thread::sleep_for(std::chrono::milliseconds(config_.reconnectDelayMs));
    } catch (const std::exception& error) {
      if (!running_.load()) {
        break;
      }

      spdlog::error("[OperationCmdListener] 运行异常: {}", error.what());
      std::this_thread::sleep_for(std::chrono::milliseconds(config_.reconnectDelayMs));
    }
  }

  subscriber_.reset();
  redis_.reset();
}

void RedisCommandSubscriber::Stop() {
  running_.store(false);

  if (subscriber_.has_value()) {
    try {
      subscriber_->unsubscribe(config_.channel);
    } catch (const sw::redis::Error&) {
    }
  }
}

bool RedisCommandSubscriber::IsRunning() const {
  return running_.load();
}

sw::redis::ConnectionOptions RedisCommandSubscriber::BuildConnectionOptions() const {
  sw::redis::ConnectionOptions options;
  options.host = config_.host;
  options.port = config_.port;
  options.password = config_.password;
  options.socket_timeout = std::chrono::milliseconds(config_.socketTimeoutMs);
  return options;
}

void RedisCommandSubscriber::Connect() {
  redis_.reset();
  subscriber_.reset();

  redis_.emplace(BuildConnectionOptions());
  subscriber_.emplace(redis_->subscriber());

  subscriber_->on_message([this](std::string channel, std::string payload) {
    HandleMessage(channel, payload);
  });

  subscriber_->subscribe(config_.channel);

  spdlog::info("[OperationCmdListener] 已订阅 Redis 频道: {} @ {}:{}",
               config_.channel,
               config_.host,
               config_.port);
}

void RedisCommandSubscriber::ConsumeLoop() {
  while (running_.load()) {
    try {
      subscriber_->consume();
    } catch (const sw::redis::TimeoutError&) {
      continue;
    }
  }
}

void RedisCommandSubscriber::HandleMessage(const std::string& channel, const std::string& payload) {
  OperationCommand command = BuildFallbackCommand(channel, payload);

  try {
    command = ParseCommandMessage(channel, payload);
  } catch (const std::exception& error) {
    spdlog::warn("[OperationCmdListener] 解析消息失败，按原始消息打印: {}", error.what());
  }

  commandHandler_(command);
}

OperationCommand RedisCommandSubscriber::BuildFallbackCommand(
    const std::string& channel,
    const std::string& payload) const {
  OperationCommand command;
  command.channel = channel;
  command.rawPayload = payload;
  command.commandName = "raw_message";
  command.commandParameterText = payload;

  try {
    command.commandParameter = nlohmann::json::parse(payload);
  } catch (...) {
    command.commandParameter = nlohmann::json::object();
  }

  return command;
}

OperationCommand RedisCommandSubscriber::ParseCommandMessage(
    const std::string& channel,
    const std::string& payload) const {
  const auto message = nlohmann::json::parse(payload);

  if (!message.contains("cmd_name") || !message["cmd_name"].is_string()) {
    throw std::runtime_error("消息缺少字符串类型的 cmd_name");
  }

  OperationCommand command;
  command.channel = channel;
  command.rawPayload = payload;
  command.commandName = message["cmd_name"].get<std::string>();

  if (!message.contains("cmd_para") || message["cmd_para"].is_null()) {
    command.commandParameter = nlohmann::json::object();
    command.commandParameterText = "{}";
    return command;
  }

  if (message["cmd_para"].is_string()) {
    command.commandParameterText = message["cmd_para"].get<std::string>();

    if (command.commandParameterText.empty()) {
      command.commandParameter = nlohmann::json::object();
      return command;
    }

    command.commandParameter = nlohmann::json::parse(command.commandParameterText);
    return command;
  }

  command.commandParameter = message["cmd_para"];
  command.commandParameterText = command.commandParameter.dump();
  return command;
}
