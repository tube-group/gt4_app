// CommL3Context.h
// 将所有工位对象和共享资源收敛到一个上下文结构体中，
// 在 main() 中创建，通过引用传递给需要的模块。
#pragma once

#include <sw/redis++/redis++.h>
// #include <pqxx/pqxx>
#include "gauss_connection.h"
#include "logging.h"   // spdlog
#include "higplat.h"
#include <memory>

struct CommL3Context {
    // 共享资源
    std::unique_ptr<sw::redis::Redis> redis;
    int gplatConn = -1;
    // std::unique_ptr<pqxx::connection> pgConn;
    std::unique_ptr<GaussDB::Connection> gaussConn;

    std::atomic_bool running{true};
    std::string weightTcpHost = "140.32.1.185";
    int weightTcpPort = 4001;

    // 统一初始化入口。
    // 当前留空，便于后续把分散初始化逻辑收敛到上下文层。
    void Init()
    {
    }

    // 清理资源
    void Cleanup() {
        gaussConn.reset();
        if (gplatConn > 0) {
            disconnectgplat(gplatConn);
            gplatConn = -1;
        }
        redis.reset();
        // pgConn.reset();
    }
};