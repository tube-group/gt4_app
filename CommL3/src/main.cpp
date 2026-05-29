#include <iostream>
#include <thread>
#include <chrono>
#include "TubeTrackContext.h"
#include <sw/redis++/redis++.h>
#include <unistd.h>    // sleep, fork, setsid, getpid, close, dup2
#include <fcntl.h>     // open, O_WRONLY, O_CREAT, O_RDWR
#include <sys/file.h>  // flock, LOCK_EX, LOCK_NB, LOCK_UN
#include <sys/stat.h>  // umask
#include <limits.h>    // PATH_MAX
#include <getopt.h>    // getopt
#include <csignal>     // signal, SIGINT, SIGTERM
#include "iniconfig.h" // CConfig
#include "logging.h"   // LogConfig

#include "gauss_loader.h"
#include <iostream>

// 前向声明
void workThread(TubeTrackContext& ctx);

// ---- 信号处理 ----
volatile sig_atomic_t g_running = 1;

static void signalHandler(int sig)
{
    (void)sig;
    g_running = 0;
}

// ---- PID文件管理 ----
static int g_pidfile_fd = -1;   // PID文件描述符，用于文件锁
static std::string g_pidfile_path;

// 打开PID文件并加锁（daemon化之前调用，验证可写性和单实例）
// 返回: true=成功, false=失败
static bool lockPidfile(const std::string& path)
{
    g_pidfile_path = path;

    g_pidfile_fd = open(path.c_str(), O_WRONLY | O_CREAT, 0644);
    if (g_pidfile_fd == -1) {
        fprintf(stderr, "Cannot open PID file %s: %s\n", path.c_str(), strerror(errno));
        return false;
    }

    // 非阻塞排他锁，如果已被锁定说明有另一个实例在运行
    if (flock(g_pidfile_fd, LOCK_EX | LOCK_NB) == -1) {
        fprintf(stderr, "Another instance is already running (PID file locked: %s)\n", path.c_str());
        close(g_pidfile_fd);
        g_pidfile_fd = -1;
        return false;
    }

    return true;
}

// 写入PID到已锁定的文件（daemon化之后调用，写子进程PID）
static bool writePidfile()
{
    if (g_pidfile_fd == -1) return false;

    if (ftruncate(g_pidfile_fd, 0) == -1) {
        return false;
    }

    char buf[32];
    int len = snprintf(buf, sizeof(buf), "%d\n", getpid());
    if (write(g_pidfile_fd, buf, len) != len) {
        return false;
    }

    return true;
}

// 清理PID文件
static void removePidfile()
{
    if (g_pidfile_fd != -1) {
        flock(g_pidfile_fd, LOCK_UN);
        close(g_pidfile_fd);
        g_pidfile_fd = -1;
    }
    if (!g_pidfile_path.empty()) {
        unlink(g_pidfile_path.c_str());
        g_pidfile_path.clear();
    }
}

// ---- 守护进程 ----
// 参考 gplat/ngx_daemon.cxx 风格: fork + setsid + umask + 重定向stdio到/dev/null
// 返回: 0=子进程(成功), 1=父进程(应退出), -1=失败
static int becomeDaemon()
{
    switch (fork()) {
    case -1:
        fprintf(stderr, "becomeDaemon(): fork() failed: %s\n", strerror(errno));
        return -1;
    case 0:
        // 子进程
        break;
    default:
        // 父进程，返回1让调用者退出
        return 1;
    }

    // 脱离终端，创建新会话
    if (setsid() == -1) {
        fprintf(stderr, "becomeDaemon(): setsid() failed: %s\n", strerror(errno));
        return -1;
    }

    // 不限制文件权限
    umask(0);

    // 重定向stdin/stdout/stderr到/dev/null
    int fd = open("/dev/null", O_RDWR);
    if (fd == -1) {
        fprintf(stderr, "becomeDaemon(): open(\"/dev/null\") failed: %s\n", strerror(errno));
        return -1;
    }
    if (dup2(fd, STDIN_FILENO) == -1) {
        fprintf(stderr, "becomeDaemon(): dup2(STDIN) failed: %s\n", strerror(errno));
        return -1;
    }
    if (dup2(fd, STDOUT_FILENO) == -1) {
        fprintf(stderr, "becomeDaemon(): dup2(STDOUT) failed: %s\n", strerror(errno));
        return -1;
    }
    if (dup2(fd, STDERR_FILENO) == -1) {
        fprintf(stderr, "becomeDaemon(): dup2(STDERR) failed: %s\n", strerror(errno));
        return -1;
    }
    if (fd > STDERR_FILENO) {
        close(fd);
    }

    return 0; // 子进程
}

// ---- 应用配置 ----
struct AppConfig {
    LogConfig logCfg;
    bool daemonMode = false;
    std::string pidFile;
};

// 加载配置文件 + 解析命令行参数
static bool loadConfig(int argc, char* argv[], AppConfig& app)
{
    auto &config = CConfig::GetInstance();
    std::string configFile = "../config/comml3.ini";
    if (!config.Load(configFile))
    {
        fprintf(stderr, "Failed to load config file: %s\n", configFile.c_str());
        return false;
    }

    app.logCfg.log_console     = config.GetBoolDefault("log_console", false);
    app.logCfg.level           = config.GetStringDefault("level", app.logCfg.level);
    app.logCfg.pattern         = config.GetStringDefault("pattern", app.logCfg.pattern);
    app.logCfg.filename        = config.GetStringDefault("filename", "log/tubetrack.log");
    app.logCfg.immediate_flush = config.GetBoolDefault("immediate_flush", app.logCfg.immediate_flush);
    app.logCfg.max_size_mb     = config.GetIntDefault("max_size", app.logCfg.max_size_mb);
    app.logCfg.max_files       = config.GetIntDefault("max_files", app.logCfg.max_files);
    app.daemonMode = config.GetBoolDefault("daemon", false);
    app.pidFile    = config.GetStringDefault("pid_file", "/var/run/comml3.pid");

    // 解析命令行参数（-d 强制守护进程模式）
    int opt;
    while ((opt = getopt(argc, argv, "dc:h")) != -1) {
        switch (opt) {
        case 'd':
            app.daemonMode = true;
            break;
        default:
            break;
        }
    }

    if (app.daemonMode) {
        fprintf(stdout, "以守护进程运行\n");
    } else {
        fprintf(stdout, "以普通进程运行\n");
    }

    return true;
}

// ---- 守护进程化（统一入口） ----
// 合并路径转换、PID文件锁定、fork、写PID
// 返回: 0=子进程继续, 1=父进程应退出, -1=失败
static int daemonize(AppConfig& app)
{
    // 将相对路径转为绝对路径（daemon后工作目录可能改变）
    auto toAbsPath = [](std::string& path) {
        if (!path.empty() && path[0] != '/') {
            char cwd[PATH_MAX];
            if (getcwd(cwd, sizeof(cwd))) {
                path = std::string(cwd) + "/" + path;
            }
        }
    };
    toAbsPath(app.logCfg.filename);
    toAbsPath(app.pidFile);

    // 打开并锁定PID文件（验证可写性和单实例）
    if (!lockPidfile(app.pidFile)) {
        return -1;
    }

    // fork + setsid + 重定向
    int rc = becomeDaemon();
    if (rc == -1) {
        fprintf(stderr, "Failed to daemonize. Exiting.\n");
        return -1;
    }

    if (rc == 1) {
        // 父进程，正常退出（不清理PID文件，由子进程持有锁）
        fprintf(stdout, "父进程，正常退出\n");
        if (g_pidfile_fd != -1) {
            close(g_pidfile_fd);
            g_pidfile_fd = -1;
        }
        return 1;
    }

    // 子进程继续，写入子进程PID
    writePidfile();

    return 0;
}

// ---- Redis连接 ----
static bool initRedis(TubeTrackContext& ctx)
{
    auto &config = CConfig::GetInstance();
    try {
        sw::redis::ConnectionOptions opts;
        opts.host = config.GetStringDefault("redis_host", "127.0.0.1");
        opts.port = config.GetIntDefault("redis_port", 6379);
        opts.password = config.GetStringDefault("redis_password", "");

        ctx.redis = std::make_unique<sw::redis::Redis>(opts);
        ctx.redis->ping();
        spdlog::info("成功连接到 Redis");
        return true;

    } catch (const std::exception& e) {
        spdlog::error("Redis连接失败: {}", e.what());
        return false;
    }
}

// ---- gplat连接 ----
static bool initGplat(TubeTrackContext& ctx)
{
    auto &config = CConfig::GetInstance();
    try {
        std::string host = config.GetStringDefault("gplat_host", "127.0.0.1");
        int port = config.GetIntDefault("gplat_port", 8777);

        int conn = connectgplat(host.c_str(), port);

        if (conn <= 0) {
            spdlog::error("gPlat连接失败");
            return false;
        }

        ctx.gplatConn = conn;
        spdlog::info("成功连接到 gPlat");
        return true;

    } catch (const std::exception& e) {
        spdlog::error("gPlat连接失败: {}", e.what());
        return false;
    }
}

// ---- PostgreSQL连接 ----
static bool initPostgreSQL(TubeTrackContext& ctx)
{
    auto &config = CConfig::GetInstance();
    try {
        // 读取PostgreSQL连接参数
        std::string dbname = config.GetStringDefault("dbname", "mesl2");
        std::string user = config.GetStringDefault("user", "l2user");
        std::string password = config.GetStringDefault("password", "");
        std::string hostaddr = config.GetStringDefault("hostaddr", "127.0.0.1");
        int port = config.GetIntDefault("port", 5432);

        // 构建连接字符串
        std::string connStr = "dbname=" + dbname +
                              " user=" + user +
                              " password=" + password +
                              " hostaddr=" + hostaddr +
                              " port=" + std::to_string(port);

        ctx.pgConn = std::make_unique<pqxx::connection>(connStr);
        
        if (ctx.pgConn->is_open()) {
            spdlog::info("成功连接到 PostgreSQL 数据库: {}", dbname);
            return true;
        } else {
            spdlog::error("PostgreSQL 连接失败: 数据库未打开");
            return false;
        }

    } catch (const std::exception& e) {
        spdlog::error("PostgreSQL 连接失败: {}", e.what());
        return false;
    }
}

// ---- 高斯数据库连接 ----
static bool initGauss(TubeTrackContext& ctx)
{
    auto &config = CConfig::GetInstance();
    try {
        std::string soPath = config.GetStringDefault(
            "gauss_so_path",
            "/app/projects/gt4_app/third_party/gauss_sdk/lib/libpq.so"
        );
        std::string host = config.GetStringDefault("gauss_host", "10.81.57.151");
        std::string dbname = config.GetStringDefault("gauss_dbname", "gfhgot");
        std::string user = config.GetStringDefault("gauss_user", "hfwot");
        std::string password = config.GetStringDefault("gauss_password", "fhq6OPx92T");
        int port = config.GetIntDefault("gauss_port", 8000);

        std::string connStr = "host=" + host +
                              " port=" + std::to_string(port) +
                              " dbname=" + dbname +
                              " user=" + user +
                              " password=" + password;

        ctx.gaussLoader = std::make_unique<GaussLoader>(soPath);
        ctx.gaussConn = ctx.gaussLoader->PQconnectdb(connStr.c_str());
        if (ctx.gaussConn == nullptr) {
            spdlog::error("高斯数据库连接失败: PQconnectdb 返回空指针");
            ctx.gaussLoader.reset();
            return false;
        }

        PGresult* res = ctx.gaussLoader->PQexec(ctx.gaussConn, "SELECT version();");
        if (res == nullptr) {
            const char* err = ctx.gaussLoader->PQerrorMessage != nullptr
                ? ctx.gaussLoader->PQerrorMessage(ctx.gaussConn)
                : nullptr;
            spdlog::error("高斯数据库连接失败: {}", err != nullptr ? err : "未知错误");
            ctx.gaussLoader->PQfinish(ctx.gaussConn);
            ctx.gaussConn = nullptr;
            ctx.gaussLoader.reset();
            return false;
        }

        bool ok = ctx.gaussLoader->PQresultStatus(res) == PGRES_TUPLES_OK;
        if (ok) {
            spdlog::info("成功连接到高斯数据库: {}", dbname);
        } else {
            const char* err = ctx.gaussLoader->PQerrorMessage != nullptr
                ? ctx.gaussLoader->PQerrorMessage(ctx.gaussConn)
                : nullptr;
            spdlog::error("高斯数据库连接失败: {}", err != nullptr ? err : "未知错误");
        }

        ctx.gaussLoader->PQclear(res);
        if (!ok) {
            ctx.gaussLoader->PQfinish(ctx.gaussConn);
            ctx.gaussConn = nullptr;
            ctx.gaussLoader.reset();
            return false;
        }

        return true;
    } 
    catch (const std::exception& e) {
        spdlog::error("高斯数据库连接失败: {}", e.what());
        ctx.gaussConn = nullptr;
        ctx.gaussLoader.reset();
        return false;
    }
}

// ----测试高斯数据库插入、更新、删除数据的功能----
static bool testGauss(TubeTrackContext& ctx)
{
    if (ctx.gaussLoader == nullptr || ctx.gaussConn == nullptr) {
        spdlog::error("高斯CRUD测试失败: 连接未初始化");
        return false;
    }
    PGresult* res = nullptr;  // 在这里声明res变量
    // // 1. 插入数据
    // const char* insertSql = "INSERT INTO test_employee(emp_name, salary) VALUES ($1, $2);";
    // const char* insertParams[2] = {"zhangsan", "8888.88"};
    // res = ctx.gaussLoader->PQexecParams(
    //     ctx.gaussConn, insertSql, 2, nullptr, 
    //     insertParams, nullptr, nullptr, 
    //     0  // 返回二进制格式或文本格式（1=二进制，0=文本）
    // );
    // if (res && ctx.gaussLoader->PQresultStatus(res) == PGRES_COMMAND_OK) {
    //     spdlog::info("插入成功，影响行数: {}", ctx.gaussLoader->PQcmdTuples(res));
    // } else {
    //     spdlog::error("插入失败");
    //     if (res) ctx.gaussLoader->PQclear(res);
    //     return false;
    // }
    // ctx.gaussLoader->PQclear(res);

    // // 2. 更新数据
    // const char* updateSql = "UPDATE test_employee SET salary=$1 WHERE emp_name=$2;";
    // const char* updateParams[2] = {"9999.99", "zhangsan"};
    // res = ctx.gaussLoader->PQexecParams(ctx.gaussConn, updateSql, 2, nullptr,
    //                                      updateParams, nullptr, nullptr, 0);
    
    // if (res || ctx.gaussLoader->PQresultStatus(res) == PGRES_COMMAND_OK) {
    //     spdlog::info("更新成功，影响行数: {}", ctx.gaussLoader->PQcmdTuples(res));
    // } else {
    //     spdlog::error("更新失败");
    //     if (res) ctx.gaussLoader->PQclear(res);
    //     return false;
    // }
    // ctx.gaussLoader->PQclear(res);

    // 3. 删除数据
    const char* deleteSql = "DELETE FROM test_employee WHERE emp_name=$1;";
    const char* deleteParams[1] = {"zhangsan"};
    res = ctx.gaussLoader->PQexecParams(ctx.gaussConn, deleteSql, 1, nullptr,
                                         deleteParams, nullptr, nullptr, 0);
    
    if (res || ctx.gaussLoader->PQresultStatus(res) == PGRES_COMMAND_OK) {
        spdlog::info("删除成功，影响行数: {}", ctx.gaussLoader->PQcmdTuples(res));
    } else {
        spdlog::error("删除失败");
        if (res) ctx.gaussLoader->PQclear(res);
        return false;
    }
    ctx.gaussLoader->PQclear(res);
    
    spdlog::info("CRUD测试通过: 插入->更新->删除成功");
    return true;

}

// ----测试同时访问高斯数据库和PostgreSQL的功能（在工作线程中调用）----
static void testGaussAndPostgreSQL(TubeTrackContext& ctx)
{
    if (ctx.gaussLoader == nullptr || ctx.gaussConn == nullptr) {
        spdlog::error("高斯和PostgreSQL测试失败: 高斯连接未初始化");
        return;
    }
    if (ctx.pgConn == nullptr || !ctx.pgConn->is_open()) {
        spdlog::error("高斯和PostgreSQL测试失败: PostgreSQL连接未初始化");
        return;
    }

    // 准备测试数据
    std::string empName = "张三";
    std::string hireDate = "2024-01-15";
    std::string salary = "8500.50";
    std::string username = "zhang_san";
    std::string fullName = "张三";
    std::string email = "zhangsan@example.com";
    std::string phone = "13812345678";
    
    PGresult* resGauss = nullptr;
    int gaussEmpId = -1;
    int pgUserId = -1;    

    // 1、插入高斯数据库
    const char* insertGaussSql = "INSERT INTO test_employee (emp_name, hire_date, salary) VALUES ($1, $2, $3) RETURNING emp_id;";
    const char* insertGaussParams[3] = {empName.c_str(), hireDate.c_str(), salary.c_str()};
    resGauss = ctx.gaussLoader->PQexecParams(ctx.gaussConn, insertGaussSql, 3, nullptr, insertGaussParams, nullptr, nullptr, 0);
    if (resGauss == nullptr || ctx.gaussLoader->PQresultStatus(resGauss) != PGRES_TUPLES_OK) {
        const char* err = ctx.gaussLoader->PQerrorMessage != nullptr
            ? ctx.gaussLoader->PQerrorMessage(ctx.gaussConn)
            : nullptr;
        if (resGauss != nullptr) ctx.gaussLoader->PQclear(resGauss);
        spdlog::error("同时插入测试失败: 向高斯数据库插入数据失败: {}", err != nullptr ? err : "未知错误");
        return;
    }

    // 获取高斯插入的记录ID
    if (ctx.gaussLoader->PQntuples(resGauss) > 0) {
        gaussEmpId = std::stoi(ctx.gaussLoader->PQgetvalue(resGauss, 0, 0));
        spdlog::info("高斯数据库插入成功，emp_id={}", gaussEmpId);
    }
    ctx.gaussLoader->PQclear(resGauss);

    // 2. 插入PostgreSQL数据库
     try {
        pqxx::work txn(*ctx.pgConn);

        std::string insertPgSql = "INSERT INTO users (username, full_name, email, phone) VALUES ($1, $2, $3, $4) RETURNING id;";
        pqxx::result resPg = txn.exec_params(insertPgSql, username, fullName, email, phone);
        
        if (!resPg.empty()) {
            pgUserId = resPg[0][0].as<int>();
            spdlog::info("PostgreSQL数据库插入成功，user_id={}", pgUserId);
        }
        txn.commit();
        
    } catch (const std::exception& e) {
        spdlog::error("同时插入测试失败: 向PostgreSQL数据库插入数据失败: {}", e.what());
        
        // 如果PostgreSQL插入失败，回滚高斯数据库的插入,保证要么两个数据库都插入成功，要么都不插入
        // 注意：这里的回滚是通过删除之前插入的记录实现的，前提是emp_id是唯一标识
        if (gaussEmpId != -1) {
            const char* rollbackSql = "DELETE FROM test_employee WHERE emp_id = $1;";
            const char* rollbackParams[1] = {std::to_string(gaussEmpId).c_str()};
            PGresult* resRollback = ctx.gaussLoader->PQexecParams(
                ctx.gaussConn, rollbackSql, 1, nullptr,
                rollbackParams, nullptr, nullptr, 0);
            if (resRollback != nullptr) ctx.gaussLoader->PQclear(resRollback);
            spdlog::warn("已回滚高斯数据库插入的记录, emp_id={}", gaussEmpId);
        }
        return;
    }
    
    spdlog::info("高斯和PostgreSQL测试通过: 高斯记录ID={}, PostgreSQL记录ID={}", gaussEmpId, pgUserId);

}

int main(int argc, char* argv[])
{
    // 1. 加载配置 + 解析命令行
    AppConfig app;
    if (!loadConfig(argc, argv, app))
        return EXIT_FAILURE;

    // 2. 守护进程化
    if (app.daemonMode) {
        int rc = daemonize(app);
        if (rc != 0) return (rc == 1) ? 0 : 1;
    }

    // 3. 注册信号处理
    signal(SIGINT, signalHandler);
    signal(SIGTERM, signalHandler);

    // 4. 初始化日志系统
    if (!initLogging(app.logCfg))
        return EXIT_FAILURE;

    // 5. 创建上下文对象（取代全局变量）
    TubeTrackContext ctx;

    // 6. 连接 Redis
    if (!initRedis(ctx)) {
        shutdownLogging();
        return EXIT_FAILURE;
    }

    // 9. 连接高斯数据库
    if (!initGauss(ctx)) {
        ctx.Cleanup();
        shutdownLogging();
        return EXIT_FAILURE;
    }

    testGauss(ctx);
    
    // // 8. 连接PostgreSQL
    // if (!initPostgreSQL(ctx)) {
    //     ctx.Cleanup();
    //     shutdownLogging();
    //     return EXIT_FAILURE;
    // }

    // testGaussAndPostgreSQL(ctx);

    // 7. 连接 gPlat
    if (!initGplat(ctx)) {
        ctx.Cleanup();
        shutdownLogging();
        return EXIT_FAILURE;
    }

    // 启动工作线程
    std::thread workerThread(workThread, std::ref(ctx));

    // 主线程等待退出命令或信号
    while (true) {
        if (!g_running) break;
        std::this_thread::sleep_for(std::chrono::seconds(1));
    }

    // 等待所有线程结束
    if (workerThread.joinable()) {
        workerThread.join();
    }

    // 资源清理
    ctx.Cleanup();
    shutdownLogging();
    if (app.daemonMode) {
        removePidfile();
    }

    return 0;
}