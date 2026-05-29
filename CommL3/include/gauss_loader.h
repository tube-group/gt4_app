#pragma once
#include <libpq-fe.h>
#include <dlfcn.h>
#include <string>
#include <stdexcept>

class GaussLoader {
public:
    // 定义我们需要用到的高斯 C 函数指针类型
    typedef PGconn* (*PQconnectdb_t)(const char*);
    typedef void (*PQfinish_t)(PGconn*);
    typedef PGresult* (*PQexec_t)(PGconn*, const char*);
    typedef ExecStatusType (*PQresultStatus_t)(const PGresult*);
    typedef int (*PQntuples_t)(const PGresult*);  // 添加 PQntuples 类型定义
    typedef char* (*PQgetvalue_t)(const PGresult*, int, int);
    typedef void (*PQclear_t)(PGresult*);
    typedef char* (*PQerrorMessage_t)(const PGconn*);
    typedef char* (*PQcmdTuples_t)(const PGresult*);  // 新增：获取受影响行数
    typedef PGresult* (*PQexecParams_t)(PGconn*, const char*, int, const Oid*, 
                        const char* const*, const int*, const int*, int);  // 新增：参数化查询

    // 实际的函数指针变量
    PQconnectdb_t PQconnectdb = nullptr;
    PQfinish_t PQfinish = nullptr;
    PQexec_t PQexec = nullptr;
    PQresultStatus_t PQresultStatus = nullptr;
    PQntuples_t PQntuples = nullptr;  // 添加 PQntuples 函数指针
    PQgetvalue_t PQgetvalue = nullptr;
    PQclear_t PQclear = nullptr;
    PQerrorMessage_t PQerrorMessage = nullptr;
    PQcmdTuples_t PQcmdTuples = nullptr; // 新增：获取受影响行数函数指针
    PQexecParams_t PQexecParams = nullptr; // 新增：参数化查询函数指针

    // 单例模式或简单构造
    GaussLoader(const std::string& so_path) {
        void* comErrHandle = dlopen("/app/projects/gt4_app/third_party/gauss_sdk/lib/libcom_err.so.3", RTLD_NOW | RTLD_GLOBAL);
        void* cryptoHandle = dlopen("/app/projects/gt4_app/third_party/gauss_sdk/lib/libcrypto.so", RTLD_NOW | RTLD_GLOBAL);
        void* sslHandle = dlopen("/app/projects/gt4_app/third_party/gauss_sdk/lib/libssl.so", RTLD_NOW | RTLD_GLOBAL);

        // 显式、精准地只动态加载这个路径下的高斯动态库
        handle = dlopen(so_path.c_str(), RTLD_LAZY | RTLD_LOCAL | RTLD_DEEPBIND);
        if (!handle) {
            throw std::runtime_error("无法加载高斯动态库: " + std::string(dlerror()));
        }

        // 绑定函数指针
        PQconnectdb = (PQconnectdb_t)dlsym(handle, "PQconnectdb");
        PQfinish = (PQfinish_t)dlsym(handle, "PQfinish");
        PQexec = (PQexec_t)dlsym(handle, "PQexec");
        PQresultStatus = (PQresultStatus_t)dlsym(handle, "PQresultStatus");
        PQntuples = (PQntuples_t)dlsym(handle, "PQntuples");  // 绑定 PQntuples
        PQgetvalue = (PQgetvalue_t)dlsym(handle, "PQgetvalue");
        PQclear = (PQclear_t)dlsym(handle, "PQclear");
        PQerrorMessage = (PQerrorMessage_t)dlsym(handle, "PQerrorMessage");
        PQcmdTuples = (PQcmdTuples_t)dlsym(handle, "PQcmdTuples"); // 绑定获取受影响行数函数指针
        PQexecParams = (PQexecParams_t)dlsym(handle, "PQexecParams"); // 绑定参数化查询函数指针
    }

    ~GaussLoader() {
        if (handle) dlclose(handle);
    }

private:
    void* handle = nullptr;
};