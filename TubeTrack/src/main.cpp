#include <iostream>
#include "TubeTrackObject.h"
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

// ---- 信号处理 ----
static volatile sig_atomic_t g_running = 1;

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
static bool lock_pidfile(const std::string& path)
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
static bool write_pidfile()
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
static void remove_pidfile()
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

int main(int argc, char* argv[])
{
    // 1. 加载配置文件
    auto &config = CConfig::GetInstance();
    std::string configFile = "../config/tubetrack.ini";
    if (!config.Load(configFile))
    {
        fprintf(stderr, "Failed to load config file: %s\n", configFile.c_str());
        return EXIT_FAILURE;
    }

    LogConfig logCfg;
    logCfg.log_console     = config.GetBoolDefault("log_console", false);
    logCfg.level           = config.GetStringDefault("level", logCfg.level);
    logCfg.pattern         = config.GetStringDefault("pattern", logCfg.pattern);
    logCfg.filename        = config.GetStringDefault("filename", "log/tubetrack.log");
    logCfg.immediate_flush = config.GetBoolDefault("immediate_flush", logCfg.immediate_flush);
    logCfg.max_size_mb     = config.GetIntDefault("max_size", logCfg.max_size_mb);
    logCfg.max_files       = config.GetIntDefault("max_files", logCfg.max_files);
    bool daemonMode = config.GetBoolDefault("daemon", false);   // 默认前台运行
    std::string pid_file= config.GetStringDefault("pid_file", "/var/run/tubetrack.pid");

    // 2. 解析命令行参数
    int opt;
    while ((opt = getopt(argc, argv, "dc:h")) != -1) {
        switch (opt) {
        case 'd':
            daemonMode = true;
            break;
        default:
            break;
        }
    }
    if (daemonMode) {
        fprintf(stdout, "以守护进程运行\n");
    } else {
        fprintf(stdout, "以普通进程运行\n");
    }

    // daemon化之前：将相对路径转为绝对路径（daemon后工作目录可能改变）
    if (daemonMode) {
        auto to_abspath = [](std::string& path) {
            if (!path.empty() && path[0] != '/') {
                char cwd[PATH_MAX];
                if (getcwd(cwd, sizeof(cwd))) {
                    path = std::string(cwd) + "/" + path;
                }
            }
        };
        to_abspath(logCfg.filename);
        to_abspath(pid_file);
    }

    // daemon化之前：打开并锁定PID文件（验证可写性和单实例，错误可输出到终端）
    if (daemonMode) {
        if (!lock_pidfile(pid_file)) {
            return 1;
        }
    }

    // 守护进程化（在日志初始化之前）
    if (daemonMode) {
        int rc = becomeDaemon();
        if (rc == -1) {
            fprintf(stderr, "Failed to daemonize. Exiting.\n");
            return 1;
        }

        if (rc == 1) {
            fprintf(stdout, "父进程，正常退出\n");
            // 父进程，正常退出（不清理PID文件，由子进程持有锁）
            // 关闭父进程的fd副本，子进程继承了锁
            if (g_pidfile_fd != -1) {
                close(g_pidfile_fd);
                g_pidfile_fd = -1;
            }
            return 0;
        }

        // 子进程继续，写入子进程PID
        write_pidfile();
    }

    // 注册信号处理（daemon化之后，确保子进程注册）
    signal(SIGINT, signalHandler);
    signal(SIGTERM, signalHandler);

    // 3. 初始化日志系统
    if (!initLogging(logCfg))
    {
        return EXIT_FAILURE;
    }

    // 4. 连接 Redis
    spdlog::info("正在连接 Redis...");
    try {
        sw::redis::ConnectionOptions opts;
        opts.host = config.GetStringDefault("redis_host", "127.0.0.1");
        opts.port = config.GetIntDefault("redis_port", 6379);
        opts.password = config.GetStringDefault("redis_password", "");

        // 创建Redis连接并赋值给全局变量
        g_redis = std::make_unique<sw::redis::Redis>(opts);

        // 测试连接
        g_redis->ping();
        spdlog::info("成功连接到 Redis");

    } catch (const std::exception& e) {
        spdlog::error("Redis连接失败: {}", e.what());
        shutdownLogging();
        return EXIT_FAILURE;
    }

    // 5. 初始化生产计划数据
    prodPlan.order_no = "20240001";
    prodPlan.item_no = "ITEM001";
    prodPlan.roll_no = "ROLL1234";
    prodPlan.melt_no = "MELT5678";
    prodPlan.lot_no = "LOT91011";
    prodPlan.lotno_coupling = "COUPLELOT";
    prodPlan.meltno_coupling = "COUPLEMELT";
    prodPlan.feed_num = 100;  // 初始投料支数
    prodPlan.tube_no = 0;     // 初始管号

    // 6. 初始同步到Redis
    prodPlan.UpdateForm();

    // 7. 模拟生产流程
    CTube tube;
    for (int i = 0; i < 100 && g_running; i++)
    {
        // 从投料计划中获取管子数据
        if (prodPlan.Pop(&tube))
        {
            // 将管子数据推送到测长工位
            if (lengthPos.Push(tube))
            {
                // 成功推送后，输出工位状态
                lengthPos.DebugOut();

                // 从测长工位弹出管子数据
                lengthPos.Pop(&tube);
            }

            // 更新Redis数据
            prodPlan.UpdateForm();
        }

        sleep(1); // 模拟生产节奏
    }

    // 8. 资源清理
    g_redis.reset();
    shutdownLogging();
    if (daemonMode) {
        remove_pidfile();
    }

    return 0;
}