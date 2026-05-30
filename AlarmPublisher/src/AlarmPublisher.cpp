#include "AlarmPublisher.h"

#include <exception>
#include <sstream>
#include <utility>

#include <nlohmann/json.hpp>
#include <sw/redis++/redis++.h>

#include "logging.h"

namespace {

using json = nlohmann::json;

constexpr const char *kAlarmChannel = "AlarmChanged";

std::string BuildPublishContext(const char *eventType,
                                const std::string &alarmCode,
                                const std::string &sourceModule,
                                const std::string &dedupeKey)
{
    std::ostringstream stream;
    stream << "eventType=" << eventType
           << ", alarmCode=" << alarmCode
           << ", sourceModule=" << sourceModule
           << ", dedupeKey=" << dedupeKey;
    return stream.str();
}

std::string ValidateRaiseRequest(const AlarmRaiseRequest &request)
{
    if (request.alarmCode.empty()) return "alarmCode is required";
    if (request.areaCode.empty()) return "areaCode is required";
    if (request.severity.empty()) return "severity is required";
    if (request.sourceModule.empty()) return "sourceModule is required";
    if (request.sourceKey.empty()) return "sourceKey is required";
    if (request.title.empty()) return "title is required";
    if (request.message.empty()) return "message is required";
    if (request.dedupeKey.empty()) return "dedupeKey is required";
    if (request.occurredAt.empty()) return "occurredAt is required";
    return {};
}

std::string ValidateClearRequest(const AlarmClearRequest &request)
{
    if (request.alarmCode.empty()) return "alarmCode is required";
    if (request.areaCode.empty()) return "areaCode is required";
    if (request.sourceModule.empty()) return "sourceModule is required";
    if (request.sourceKey.empty()) return "sourceKey is required";
    if (request.dedupeKey.empty()) return "dedupeKey is required";
    if (request.occurredAt.empty()) return "occurredAt is required";
    return {};
}

bool ParseDetailJson(const std::string &detailJsonText, json &detailJson, std::string &errorMessage)
{
    if (detailJsonText.empty())
    {
        detailJson = json::object();
        return true;
    }

    detailJson = json::parse(detailJsonText, nullptr, false);
    if (detailJson.is_discarded())
    {
        errorMessage = "detailJson is not valid JSON";
        return false;
    }

    return true;
}

json BuildRaisePayload(const AlarmRaiseRequest &request, const json &detailJson)
{
    return json{
        {"alarmCode", request.alarmCode},
        {"areaCode", request.areaCode},
        {"severity", request.severity},
        {"sourceModule", request.sourceModule},
        {"sourceKey", request.sourceKey},
        {"title", request.title},
        {"message", request.message},
        {"detailJson", detailJson},
        {"requireAck", request.requireAck},
        {"autoClear", request.autoClear},
        {"dedupeKey", request.dedupeKey},
        {"occurredAt", request.occurredAt},
        {"eventType", "raise"}
    };
}

json BuildClearPayload(const AlarmClearRequest &request, const json &detailJson)
{
    return json{
        {"alarmCode", request.alarmCode},
        {"areaCode", request.areaCode},
        {"severity", request.severity},
        {"sourceModule", request.sourceModule},
        {"sourceKey", request.sourceKey},
        {"title", request.title},
        {"message", request.message},
        {"detailJson", detailJson},
        {"dedupeKey", request.dedupeKey},
        {"occurredAt", request.occurredAt},
        {"eventType", "clear"}
    };
}

} // namespace

AlarmPublishResult AlarmPublishResult::Success(std::string eventKey, std::string channel)
{
    AlarmPublishResult result;
    result.ok = true;
    result.eventKey = std::move(eventKey);
    result.channel = std::move(channel);
    return result;
}

AlarmPublishResult AlarmPublishResult::Failure(std::string errorMessage, std::string eventKey, std::string channel)
{
    AlarmPublishResult result;
    result.ok = false;
    result.eventKey = std::move(eventKey);
    result.channel = std::move(channel);
    result.errorMessage = std::move(errorMessage);
    return result;
}

AlarmPublisher::AlarmPublisher(sw::redis::Redis &redis)
    : redis_(redis)
{
}

AlarmPublishResult AlarmPublisher::Raise(const AlarmRaiseRequest &request)
{
    const std::string eventKey = BuildEventKey(request.dedupeKey);
    const std::string context = BuildPublishContext("raise", request.alarmCode, request.sourceModule, request.dedupeKey);

    const std::string validationError = ValidateRaiseRequest(request);
    if (!validationError.empty())
    {
        const std::string errorMessage = "AlarmPublisher::Raise validation failed: " + validationError + ", " + context;
        spdlog::error(errorMessage);
        return AlarmPublishResult::Failure(errorMessage, eventKey, kAlarmChannel);
    }

    try
    {
        json detailJson;
        std::string detailError;
        if (!ParseDetailJson(request.detailJson, detailJson, detailError))
        {
            const std::string errorMessage = "AlarmPublisher::Raise validation failed: " + detailError + ", " + context;
            spdlog::error(errorMessage);
            return AlarmPublishResult::Failure(errorMessage, eventKey, kAlarmChannel);
        }

        const json payload = BuildRaisePayload(request, detailJson);
        redis_.set(eventKey, payload.dump());
        redis_.publish(kAlarmChannel, eventKey);

        spdlog::info("AlarmPublished: {}, eventKey={}", context, eventKey);
        return AlarmPublishResult::Success(eventKey, kAlarmChannel);
    }
    catch (const std::exception &ex)
    {
        const std::string errorMessage = "AlarmPublisher::Raise failed: " + context + ", error=" + ex.what();
        spdlog::error(errorMessage);
        return AlarmPublishResult::Failure(errorMessage, eventKey, kAlarmChannel);
    }
}

AlarmPublishResult AlarmPublisher::Clear(const AlarmClearRequest &request)
{
    const std::string eventKey = BuildEventKey(request.dedupeKey);
    const std::string context = BuildPublishContext("clear", request.alarmCode, request.sourceModule, request.dedupeKey);

    const std::string validationError = ValidateClearRequest(request);
    if (!validationError.empty())
    {
        const std::string errorMessage = "AlarmPublisher::Clear validation failed: " + validationError + ", " + context;
        spdlog::error(errorMessage);
        return AlarmPublishResult::Failure(errorMessage, eventKey, kAlarmChannel);
    }

    try
    {
        json detailJson;
        std::string detailError;
        if (!ParseDetailJson(request.detailJson, detailJson, detailError))
        {
            const std::string errorMessage = "AlarmPublisher::Clear validation failed: " + detailError + ", " + context;
            spdlog::error(errorMessage);
            return AlarmPublishResult::Failure(errorMessage, eventKey, kAlarmChannel);
        }

        const json payload = BuildClearPayload(request, detailJson);
        redis_.set(eventKey, payload.dump());
        redis_.publish(kAlarmChannel, eventKey);

        spdlog::info("AlarmPublished: {}, eventKey={}", context, eventKey);
        return AlarmPublishResult::Success(eventKey, kAlarmChannel);
    }
    catch (const std::exception &ex)
    {
        const std::string errorMessage = "AlarmPublisher::Clear failed: " + context + ", error=" + ex.what();
        spdlog::error(errorMessage);
        return AlarmPublishResult::Failure(errorMessage, eventKey, kAlarmChannel);
    }
}

std::string AlarmPublisher::BuildEventKey(const std::string &dedupeKey)
{
    return "alarm:event:" + dedupeKey;
}