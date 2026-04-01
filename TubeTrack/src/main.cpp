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
#include "../../include/iniconfig.h" // CConfig
#include "../../include/logging.h"   // LogConfig

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
    std::string configFile = "../config/tubetrack.ini";
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
    app.pidFile    = config.GetStringDefault("pid_file", "/var/run/tubetrack.pid");

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

    // 7. 连接 gPlat
    if (!initGplat(ctx)) {
        ctx.Cleanup();
        shutdownLogging();
        return EXIT_FAILURE;
    }

    // 8. 统一注入上下文到所有工位
    ctx.Init();

    // 9. 初始化生产计划数据
    ctx.prodPlan.order_no = "20240001";
    ctx.prodPlan.item_no = "ITEM001";
    ctx.prodPlan.roll_no = "ROLL1234";
    ctx.prodPlan.melt_no = "MELT5678";
    ctx.prodPlan.lot_no = "LOT91011";
    ctx.prodPlan.lotno_coupling = "COUPLELOT";
    ctx.prodPlan.meltno_coupling = "COUPLEMELT";
    ctx.prodPlan.feed_num = 100;  // 初始投料支数
    ctx.prodPlan.tube_no = 0;     // 初始管号

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