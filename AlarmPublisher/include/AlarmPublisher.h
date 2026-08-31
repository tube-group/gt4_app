#pragma once

#include <string>

namespace pqxx {
class connection;
}

namespace sw::redis {
class Redis;
}

class AlarmPublisher
{
public:
    AlarmPublisher(pqxx::connection &database, sw::redis::Redis &redis);

    bool publish(const std::string &message, const std::string &area = "");

private:
    pqxx::connection &database_;
    sw::redis::Redis &redis_;
};