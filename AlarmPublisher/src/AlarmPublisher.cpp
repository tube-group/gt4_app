#include "AlarmPublisher.h"

#include <algorithm>
#include <cctype>
#include <exception>
#include <stdexcept>
#include <string_view>

#include <pqxx/pqxx>
#include <sw/redis++/redis++.h>

#include "logging.h"

namespace {

constexpr const char *kAlarmChannel = "AlarmChanged";

std::string Trim(const std::string &value)
{
    const auto isWhitespace = [](unsigned char character) { return std::isspace(character) != 0; };
    const auto first = std::find_if_not(value.begin(), value.end(), isWhitespace);
    const auto last = std::find_if_not(value.rbegin(), value.rend(), isWhitespace).base();
    return first < last ? std::string(first, last) : std::string{};
}

std::size_t Utf8CharacterCount(std::string_view value)
{
    return static_cast<std::size_t>(std::count_if(value.begin(), value.end(), [](unsigned char byte) {
        return (byte & 0xC0U) != 0x80U;
    }));
}

} // namespace

AlarmPublisher::AlarmPublisher(pqxx::connection &database, sw::redis::Redis &redis)
    : database_(database), redis_(redis)
{
    pqxx::nontransaction transaction(database_);
    const pqxx::result result = transaction.exec(
        "SELECT to_regclass('public.alarm_event') IS NOT NULL AS exists");
    if (result.empty() || !result[0]["exists"].as<bool>())
    {
        throw std::runtime_error("required table public.alarm_event does not exist");
    }
}

bool AlarmPublisher::publish(const std::string &message, const std::string &area)
{
    const std::string normalizedMessage = Trim(message);
    const std::string normalizedArea = Trim(area);
    if (normalizedMessage.empty())
    {
        spdlog::error("AlarmPublisher::publish validation failed: message is required");
        return false;
    }
    if (Utf8CharacterCount(normalizedArea) > 100)
    {
        spdlog::error("AlarmPublisher::publish validation failed: area exceeds 100 characters");
        return false;
    }

    std::string eventId;
    try
    {
        pqxx::work transaction(database_);
        const pqxx::result result = transaction.exec_params(
            "INSERT INTO public.alarm_event (message, area) VALUES ($1, $2) RETURNING id::text",
            normalizedMessage,
            normalizedArea);
        eventId = result[0][0].as<std::string>();
        transaction.commit();
    }
    catch (const std::exception &exception)
    {
        spdlog::error("AlarmPublisher::publish database write failed: {}", exception.what());
        return false;
    }

    try
    {
        redis_.publish(kAlarmChannel, eventId);
    }
    catch (const std::exception &exception)
    {
        spdlog::error("AlarmPublisher::publish notification failed: eventId={}, error={}", eventId, exception.what());
    }

    spdlog::info("AlarmPublished: eventId={}, area={}", eventId, normalizedArea);
    return true;
}