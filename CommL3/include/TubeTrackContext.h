// TubeTrackContext.h
// 将所有工位对象和共享资源收敛到一个上下文结构体中，
// 在 main() 中创建，通过引用传递给需要的模块。
#pragma once

#include <sw/redis++/redis++.h>
#include <pqxx/pqxx>
#include "logging.h"   // spdlog
#include "higplat.h"
#include <memory>

struct TubeTrackContext {
    // 共享资源
    std::unique_ptr<sw::redis::Redis> redis;
    int gplatConn = -1;
    std::unique_ptr<pqxx::connection> pgConn;

    // 清理资源
    void Cleanup() {
        if (gplatConn > 0) {
            disconnectgplat(gplatConn);
            gplatConn = -1;
        }
        redis.reset();
        pgConn.reset();
    }
};
