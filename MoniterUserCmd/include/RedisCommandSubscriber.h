#pragma once

#include <atomic>
#include <functional>
#include <optional>
#include <string>

#include <nlohmann/json.hpp>
#include <sw/redis++/redis++.h>

struct RedisSubscriberConfig {
  std::string host = "127.0.0.1";
  int port = 6379;
  std::string password;
  std::string channel = "operation_cmd";
  int socketTimeoutMs = 1000;
  int reconnectDelayMs = 2000;
};

struct OperationCommand {
  std::string channel;
  std::string rawPayload;
  std::string commandName;
  std::string commandParameterText;
  nlohmann::json commandParameter;
};

class RedisCommandSubscriber {
 public:
  using CommandHandler = std::function<void(const OperationCommand& command)>;

  explicit RedisCommandSubscriber(RedisSubscriberConfig config);

  void SetCommandHandler(CommandHandler handler);
  void Start();
  void Stop();
  bool IsRunning() const;

 private:
  sw::redis::ConnectionOptions BuildConnectionOptions() const;
  void Connect();
  void ConsumeLoop();
  void HandleMessage(const std::string& channel, const std::string& payload);
  OperationCommand BuildFallbackCommand(const std::string& channel, const std::string& payload) const;
  OperationCommand ParseCommandMessage(const std::string& channel, const std::string& payload) const;

  RedisSubscriberConfig config_;
  CommandHandler commandHandler_;
  std::atomic_bool running_{false};
  std::optional<sw::redis::Redis> redis_;
  std::optional<sw::redis::Subscriber> subscriber_;
};