#pragma once

#include <atomic>
#include <memory>

#include <sw/redis++/redis++.h>
#include <pqxx/pqxx>

#include "higplat.h"
#include "types.h"


struct SprayWeightContext {

    std::unique_ptr<sw::redis::Redis> redis;
    int gplatConn = -1;
    std::unique_ptr<pqxx::connection> pgConn;
    std::atomic_bool running{true};
    // 模块完整配置快照。
    SprayWeightAppConfig app;

    // 统一初始化入口。
    // 当前留空，便于后续把分散初始化逻辑收敛到上下文层。
    void Init() {
    }

    // 统一资源释放入口。
    // 释放顺序以外部连接为主，避免线程退出后仍持有失效句柄。
    void Cleanup() {
        if (gplatConn > 0) {
            disconnectgplat(gplatConn);
            gplatConn = -1;
        }
        if (pgConn) {
            pgConn.reset();
        }
        redis.reset();
        pgConn.reset();
    }
};

