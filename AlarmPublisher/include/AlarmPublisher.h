#pragma once

#include <string>

namespace sw::redis {
class Redis;
}

struct AlarmRaiseRequest
{
    std::string alarmCode;
    std::string areaCode;
    std::string severity;
    std::string sourceModule;
    std::string sourceKey;
    std::string title;
    std::string message;
    std::string detailJson = "{}";
    bool requireAck = true;
    bool autoClear = false;
    std::string dedupeKey;
    std::string occurredAt;
};

struct AlarmClearRequest
{
    std::string alarmCode;
    std::string areaCode;
    std::string sourceModule;
    std::string sourceKey;
    std::string dedupeKey;
    std::string occurredAt;
    std::string severity;
    std::string title;
    std::string message;
    std::string detailJson = "{}";
};

struct AlarmPublishResult
{
    bool ok = false;
    std::string eventKey;
    std::string channel;
    std::string errorMessage;

    static AlarmPublishResult Success(std::string eventKey, std::string channel = "AlarmChanged");
    static AlarmPublishResult Failure(std::string errorMessage, std::string eventKey = {}, std::string channel = "AlarmChanged");
};

class AlarmPublisher
{
public:
    explicit AlarmPublisher(sw::redis::Redis &redis);

    AlarmPublishResult Raise(const AlarmRaiseRequest &request);
    AlarmPublishResult Clear(const AlarmClearRequest &request);

private:
    sw::redis::Redis &redis_;

    static std::string BuildEventKey(const std::string &dedupeKey);
};