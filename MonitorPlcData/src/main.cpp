#include <chrono>
#include <csignal>
#include <cstring>
#include <iostream>
#include <thread>

#include <fcntl.h>
#include <getopt.h>
#include <limits.h>
#include <sys/file.h>
#include <sys/stat.h>
#include <unistd.h>

#include "iniconfig.h"
#include "workthread.h"

// 前向声明
static void normalizeRuntimePaths(AppConfig &app);

// ---- 信号处理 ----
volatile sig_atomic_t g_running = 1;

static void signalHandler(int sig)
{
    (void)sig;
    g_running = 0;
}

// ---- PID文件管理 ----
static int g_pidfile_fd = -1; // PID文件描述符，用于文件锁
static std::string g_pidfile_path;

// 打开PID文件并加锁（daemon化之前调用，验证可写性和单实例）
// 返回: true=成功, false=失败
static bool lockPidfile(const std::string &path)
{
    g_pidfile_path = path;

    g_pidfile_fd = open(path.c_str(), O_WRONLY | O_CREAT, 0644);
    if (g_pidfile_fd == -1)
    {
        fprintf(stderr, "Cannot open PID file %s: %s\n", path.c_str(), strerror(errno));
        return false;
    }

    // 非阻塞排他锁，如果已被锁定说明有另一个实例在运行
    if (flock(g_pidfile_fd, LOCK_EX | LOCK_NB) == -1)
    {
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
    if (g_pidfile_fd == -1)
        return false;

    if (ftruncate(g_pidfile_fd, 0) == -1)
    {
        return false;
    }

    char buf[32];
    int len = snprintf(buf, sizeof(buf), "%d\n", getpid());
    if (write(g_pidfile_fd, buf, len) != len)
    {
        return false;
    }

    return true;
}

// 清理PID文件
static void removePidfile()
{
    if (g_pidfile_fd != -1)
    {
        flock(g_pidfile_fd, LOCK_UN);
        close(g_pidfile_fd);
        g_pidfile_fd = -1;
    }
    if (!g_pidfile_path.empty())
    {
        unlink(g_pidfile_path.c_str());
        g_pidfile_path.clear();
    }
}

// ---- 守护进程 ----
// 参考 gplat/ngx_daemon.cxx 风格: fork + setsid + umask + 重定向stdio到/dev/null
// 返回: 0=子进程(成功), 1=父进程(应退出), -1=失败
static int becomeDaemon()
{
    switch (fork())
    {
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
    if (setsid() == -1)
    {
        fprintf(stderr, "becomeDaemon(): setsid() failed: %s\n", strerror(errno));
        return -1;
    }

    // 不限制文件权限
    umask(0);

    // 重定向stdin/stdout/stderr到/dev/null
    int fd = open("/dev/null", O_RDWR);
    if (fd == -1)
    {
        fprintf(stderr, "becomeDaemon(): open(\"/dev/null\") failed: %s\n", strerror(errno));
        return -1;
    }
    if (dup2(fd, STDIN_FILENO) == -1)
    {
        fprintf(stderr, "becomeDaemon(): dup2(STDIN) failed: %s\n", strerror(errno));
        return -1;
    }
    if (dup2(fd, STDOUT_FILENO) == -1)
    {
        fprintf(stderr, "becomeDaemon(): dup2(STDOUT) failed: %s\n", strerror(errno));
        return -1;
    }
    if (dup2(fd, STDERR_FILENO) == -1)
    {
        fprintf(stderr, "becomeDaemon(): dup2(STDERR) failed: %s\n", strerror(errno));
        return -1;
    }
    if (fd > STDERR_FILENO)
    {
        close(fd);
    }

    return 0; // 子进程
}

// 加载配置文件 + 解析命令行参数
static bool loadConfig(int argc, char *argv[], AppConfig &app)
{
    auto &config = CConfig::GetInstance();
    std::string configFile = "../config/MonitorPlcData.ini";
    if (!config.Load(configFile))
    {
        fprintf(stderr, "Failed to load config file: %s\n", configFile.c_str());
        return false;
    }

    app.logCfg.log_console = config.GetBoolDefault("log_console", false);
    app.logCfg.level = config.GetStringDefault("level", app.logCfg.level);
    app.logCfg.pattern = config.GetStringDefault("pattern", app.logCfg.pattern);
    app.logCfg.filename = config.GetStringDefault("filename", "log/monitorplcdata.log");
    app.logCfg.immediate_flush = config.GetBoolDefault("immediate_flush", app.logCfg.immediate_flush);
    app.logCfg.max_size_mb = config.GetIntDefault("max_size", app.logCfg.max_size_mb);
    app.logCfg.max_files = config.GetIntDefault("max_files", app.logCfg.max_files);
    app.logCfg.logger_name = "monitor_plc_data";
    app.daemonMode = config.GetBoolDefault("daemon", false);
    app.pidFile = config.GetStringDefault("pid_file", "/tmp/monitorplcdata.pid");
    app.redisChannel = config.GetStringDefault("redis_channel", config.GetStringDefault("target_channel", app.redisChannel));
    app.reconnectIntervalMs = config.GetIntDefault("reconnect_interval", app.reconnectIntervalMs);

    // 解析命令行参数（-d 强制守护进程模式）
    int opt;
    while ((opt = getopt(argc, argv, "dc:h")) != -1)
    {
        switch (opt)
        {
        case 'd':
            app.daemonMode = true;
            break;
        default:
            break;
        }
    }

    if (app.daemonMode)
    {
        fprintf(stdout, "以守护进程运行\n");
    }
    else
    {
        fprintf(stdout, "以普通进程运行\n");
    }

    return true;
}

// ---- 守护进程化（统一入口） ----
// 将运行时路径转为绝对路径，避免daemon后工作目录变化影响
static void normalizeRuntimePaths(AppConfig &app)
{
    auto toAbsPath = [](std::string &path)
    {
        if (!path.empty() && path[0] != '/')
        {
            char cwd[PATH_MAX];
            if (getcwd(cwd, sizeof(cwd)))
            {
                path = std::string(cwd) + "/" + path;
            }
        }
    };

    toAbsPath(app.logCfg.filename);
    toAbsPath(app.pidFile);
}

// ---- 守护进程化（统一入口） ----
// fork + setsid + 重定向 + 子进程写PID
// 返回: 0=子进程继续, 1=父进程应退出, -1=失败
static int daemonize(AppConfig &app)
{
    normalizeRuntimePaths(app);

    if (!lockPidfile(app.pidFile))
    {
        return -1;
    }

    // fork + setsid + 重定向
    int rc = becomeDaemon();
    if (rc == -1)
    {
        fprintf(stderr, "Failed to daemonize. Exiting.\n");
        return -1;
    }

    if (rc == 1)
    {
        // 父进程，正常退出（不清理PID文件，由子进程持有锁）
        fprintf(stdout, "父进程，正常退出\n");
        if (g_pidfile_fd != -1)
        {
            close(g_pidfile_fd);
            g_pidfile_fd = -1;
        }
        return 1;
    }

    // 子进程继续，写入子进程PID
    if (!writePidfile())
    {
        fprintf(stderr, "Failed to write PID file after daemonize.\n");
        return -1;
    }

    return 0;
}

// ---- Redis连接 ----
static bool initRedis(MonitorPlcDataContext &ctx)
{
    auto &config = CConfig::GetInstance();
    try
    {
        sw::redis::ConnectionOptions options;
        options.host = config.GetStringDefault("redis_host", "127.0.0.1");
        options.port = config.GetIntDefault("redis_port", 6379);
        options.password = config.GetStringDefault("redis_password", "");
        options.socket_timeout = std::chrono::milliseconds(1000);

        ctx.redis = std::make_unique<sw::redis::Redis>(options);
        ctx.redis->ping();
        spdlog::info("成功连接 Redis");
        return true;
    }
    catch (const std::exception &ex)
    {
        spdlog::error("Redis 连接失败: {}", ex.what());
        return false;
    }
}

// ---- gplat连接 ----
bool initGplat(MonitorPlcDataContext &ctx)
{
    auto &config = CConfig::GetInstance();
    const std::string host = config.GetStringDefault("gplat_server", config.GetStringDefault("gplat_host", "127.0.0.1"));
    const int port = config.GetIntDefault("gplat_port", 8777);

    ctx.gplatConn = connectgplat(host.c_str(), port);
    if (ctx.gplatConn <= 0)
    {
        spdlog::error("gPlat 连接失败: {}:{}", host, port);
        ctx.gplatConn = -1;
        return false;
    }

    spdlog::info("成功连接 gPlat: {}:{}", host, port);
    return true;
}

// ---- PLC 点位定义 ----
static std::vector<TagDefinition> buildStaticTags()
{
    return {
        {"ALIGN_POS_ON", "BOOL", "S7_GT4_SGAO"},          // 对齐工位占用信号 ALIGN_POS
        {"WEIGHT_POS_ON", "BOOL", "S7_GT4_SGAO"},            // 称重工位占用信号 WEI_POS
        {"CARVE_POS_ON", "BOOL", "S7_GT4_SGAO"},            // 压印工位占用信号 PRT_POS
        {"SPRAY_POS_ON", "BOOL", "S7_GT4_SGAO"},            // 喷涂工位占用信号 SPY_POS
        {"CIRCLE_POS_ON", "BOOL", "S7_GT4_SGAO"},            // 色环工位占用信号 CIR_POS
        {"LENGTH_MEASURE_FINISH", "BOOL", "S7_GT4_SGAO"},        // 来自PLC的测长完成信号 LEN_MEA_COMP
        {"WB_BASE", "BOOL", "S7_GT4_SGAO"},               // 测量点步进梁原位信号
        {"LEN_ENABLE", "BOOL", "S7_GT4_SGAO"},            // 测长允许
        {"START_CALIB", "BOOL", "S7_GT4_SGAO"},           // 标定开始
        {"WEI_ENABLE", "BOOL", "S7_GT4_SGAO"},            // 称重允许
        {"WEI_FINISH", "BOOL", "S7_GT4_SGAO"},            // 称重结束
        {"PRT_ENABLE", "BOOL", "S7_GT4_SGAO"},            // 压印允许
        {"SPY_ENABLE", "BOOL", "S7_GT4_SGAO"},            // 喷印允许
        {"CIR_ENABLE", "BOOL", "S7_GT4_SGAO"},            // 色环允许
        {"CIR1_ENABLE", "BOOL", "S7_GT4_SGAO"},           // 1号色环允许
        {"CIR2_ENABLE", "BOOL", "S7_GT4_SGAO"},           // 2号色环允许
        {"CIR3_ENABLE", "BOOL", "S7_GT4_SGAO"},           // 3号色环允许
        {"CIR4_ENABLE", "BOOL", "S7_GT4_SGAO"},           // 4号色环允许
        {"CIR5_ENABLE", "BOOL", "S7_GT4_SGAO"},           // 5号色环允许
        {"WB_RELEASE", "BOOL", "S7_GT4_SGAO"},            // 步进梁释放信号连锁，为0时才允许料流修改 WB_RELEASE
        {"LEN_POS_RDY", "BOOL", "S7_GT4_SGAO"},           // 测长工位备妥
        {"WEI_POS_RDY", "BOOL", "S7_GT4_SGAO"},           // 称重工位备妥
        {"PRT_POS_RDY", "BOOL", "S7_GT4_SGAO"},           // 压印工位备妥
        {"SPY_POS_RDY", "BOOL", "S7_GT4_SGAO"},           // 喷涂工位备妥
        {"CIR_POS_RDY", "BOOL", "S7_GT4_SGAO"},           // 色环工位备妥
        {"SCR_ROLLER_RDY", "BOOL", "S7_GT4_SGAO"},        // 出料工位备妥
        {"WB_RDY", "BOOL", "S7_GT4_SGAO"},                // 步进梁备妥
        {"SAM_LEN", "REAL", "S7_GT4_SGAO"},               // 样管长度
        {"TUBE_DIA", "REAL", "S7_GT4_SGAO"},              // 钢管外径
        {"WATCHDOG", "DINT", "S7_GT4_SGAO"},              // 通讯看门狗信号（必须是模拟量的最后一个值）
        {"SCRAPTROLLER_POS_ON", "BOOL", "S7_GT4_MainLine"},     // 涂层纵向输送链第1组钢管检测有料
        {"NBWB_RELEASE", "BOOL", "S7_GT4_MainLine"},      // 内保步进梁释放信号连锁
        {"WASTE_ROLLER_FLAG", "WORD", "S7_GT4_MainLine"}, // 涂层纵向输送链第1组钢管去向
        {"WATCHDOG2", "DINT", "S7_GT4_MainLine"}          // 通讯看门狗信号2
    };
}

// ---- 初始化点位 ----
static bool initializeTags(MonitorPlcDataContext &ctx)
{
    ctx.tags = buildStaticTags();
    ctx.tagMap.clear();
    for (const auto &tag : ctx.tags)
    {
        ctx.tagMap.emplace(tag.name, tag);
    }
    spdlog::info("已加载固定 PLC 点位 {} 个", ctx.tags.size());
    return true;
}

int main(int argc, char *argv[])
{
    // 1. 加载配置 + 解析命令行
    AppConfig app;
    if (!loadConfig(argc, argv, app))
        return EXIT_FAILURE;

    // 2. 守护进程化
    if (app.daemonMode)
    {
        int rc = daemonize(app);
        if (rc != 0)
            return (rc == 1) ? 0 : 1;
    }
    // 3. 注册信号处理
    signal(SIGINT, signalHandler);
    signal(SIGTERM, signalHandler);

    // 4. 初始化日志系统
    if (!initLogging(app.logCfg))
        return EXIT_FAILURE;

    // 5. 创建上下文对象
    MonitorPlcDataContext ctx;

    // 6. 初始化点位定义
    if (!initializeTags(ctx))
    {
        shutdownLogging();
        return EXIT_FAILURE;
    }

    // 7. 连接 Redis
    if (!initRedis(ctx))
    {
        shutdownLogging();
        return EXIT_FAILURE;
    }

    // 8. 连接 gPlat
    if (!initGplat(ctx))
    {
        ctx.Cleanup();
        shutdownLogging();
        return EXIT_FAILURE;
    }

    // 9. 启动工作线程
    std::thread workerThread(workThread, std::ref(ctx), std::cref(app), std::ref(g_running));

    // 主线程等待退出命令或信号
    while (true) {
        if (!g_running) break;
        std::this_thread::sleep_for(std::chrono::seconds(1));
    }

    // 等待所有线程结束
    if (workerThread.joinable()) {
        workerThread.join();
    }

    // 11. 资源清理
    ctx.Cleanup();
    shutdownLogging();
    if (app.daemonMode)
    {
        removePidfile();
    }
    return EXIT_SUCCESS;
}
