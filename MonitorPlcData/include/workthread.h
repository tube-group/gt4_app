#pragma once

#include <csignal>

#include "MonitorPlcDataContext.h"

struct AppConfig
{
    LogConfig logCfg;
    bool daemonMode = false;
    std::string pidFile;
    std::string redisChannel = "RealDataChanged";
    int reconnectIntervalMs = 3000;
};

bool initGplat(MonitorPlcDataContext& ctx);

void workThread(MonitorPlcDataContext& ctx, const AppConfig& app, volatile sig_atomic_t& running);