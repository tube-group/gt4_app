#pragma once

#include <string>

#include "logging.h"

// 任务队列中的业务码定义。
// 这些常量用于和 GPLAT 任务队列约定不同工序的触发类型。
constexpr int kTaskCodeWeightReady = 12;
constexpr int kTaskCodeSprayFinish = 25;

#pragma pack(push, 8)

// GPLAT 通用任务记录。
// 字段命名沿用原平台协议，不在此处做语义重命名，避免映射出错。
struct TaskRecord {
    int code = 0;
    int n0 = 0;
    int n1 = 0;
    int n2 = 0;
    int n3 = 0;
    int n4 = 0;
    int n5 = 0;
    int n6 = 0;
    int n7 = 0;
    int n8 = 0;
    int n9 = 0;
    float f0 = 0.0f;
    float f1 = 0.0f;
    double d = 0.0;
    char str0[16]{};
    char str1[16]{};
    char str2[16]{};
    char str3[16]{};
    char str4[16]{};
    char str5[24]{};
    char str6[24]{};
    char str7[24]{};
    char str8[32]{};
    char str9[32]{};
    char lstrPar[128]{};
};

// 报警队列写入结构。
// type 和 grade 保持与现有报警平台协议一致。
struct AlarmEventWriteRecord {
    int type = 1;
    int grade = 9;
    char text[96]{};
};

#pragma pack(pop)

// 应用总配置。
// 仅保留 SprayWeight.ini 对外暴露的配置项。
struct SprayWeightAppConfig {
    LogConfig logCfg;
    bool daemonMode = false;
    std::string pidFile = "/tmp/SprayWeight.pid";
    std::string gplatServer = "127.0.0.1";
    int gplatPort = 8777;
};