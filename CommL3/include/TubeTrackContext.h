// TubeTrackContext.h
// 将所有工位对象和共享资源收敛到一个上下文结构体中，
// 在 main() 中创建，通过引用传递给需要的模块。
#pragma once

#include <sw/redis++/redis++.h>
#include <pqxx/pqxx>
#include "gauss_loader.h"
#include "logging.h"   // spdlog
#include "higplat.h"
#include <memory>

struct TubeTrackContext {
    // 共享资源
    std::unique_ptr<sw::redis::Redis> redis;
    int gplatConn = -1;
    std::unique_ptr<pqxx::connection> pgConn;
    std::unique_ptr<GaussLoader> gaussLoader;
    PGconn* gaussConn = nullptr;

    // 清理资源
    void Cleanup() {
        if (gaussConn != nullptr && gaussLoader != nullptr && gaussLoader->PQfinish != nullptr) {
            gaussLoader->PQfinish(gaussConn);
            gaussConn = nullptr;
        }
        if (gplatConn > 0) {
            disconnectgplat(gplatConn);
            gplatConn = -1;
        }
        redis.reset();
        pgConn.reset();
        gaussLoader.reset();
    }
};