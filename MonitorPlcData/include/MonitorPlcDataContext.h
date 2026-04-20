#pragma once

#include <memory>
#include <string>
#include <unordered_map>
#include <vector>

#include <sw/redis++/redis++.h>

#include "higplat.h"
#include "logging.h"

// PLC点位定义
struct TagDefinition {
    std::string name;
    std::string type;
    std::string section;
};

// 应用上下文，包含所有全局资源
struct MonitorPlcDataContext {
    std::unique_ptr<sw::redis::Redis> redis; // Redis连接对象
    int gplatConn = -1;// gPlat连接句柄
    std::vector<TagDefinition> tags;// 从配置文件加载的PLC点位列表
    std::unordered_map<std::string, TagDefinition> tagMap;// 点位名称到定义的映射，便于快速查找

    void Cleanup() {
        if (gplatConn > 0) {
            disconnectgplat(gplatConn);
            gplatConn = -1;
        }
        redis.reset();
    }
};