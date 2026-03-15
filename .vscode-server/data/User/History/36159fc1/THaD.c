#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <sys/un.h>
#include <signal.h>
#include <time.h>
#include <string.h>
#include <sys/statvfs.h>

#define SOCKET_PATH "/tmp/system_info_socket"
#define INTERVAL 15  // 15秒采集周期

// 全局变量：用于信号处理退出
int server_running = 1;

// 信号处理函数：Ctrl+C/SIGTERM 触发退出
void sig_handler(int sig) {
    server_running = 0;
}

// 获取系统时间
void get_time(char *buf, size_t size) {
    time_t t = time(NULL);
    strftime(buf, size, "%Y-%m-%d %H:%M:%S", localtime(&t));
}

// 获取CPU型号
void get_cpu_info(char *buf, size_t size) {
    FILE *fp = popen("cat /proc/cpuinfo | grep 'model name' | head -1", "r");
    if (fp) {
        fgets(buf, size, fp);
        // 去除换行符
        buf[strcspn(buf, "\n")] = '\0';
        pclose(fp);
    } else {
        snprintf(buf, size, "Unknown CPU");
    }
}

// 获取操作系统版本
void get_os_info(char *buf, size_t size) {
    FILE *fp = popen("cat /etc/os-release | grep 'PRETTY_NAME' | cut -d'=' -f2", "r");
    if (fp) {
        fgets(buf, size, fp);
        buf[strcspn(buf, "\n")] = '\0';
        // 去除引号
        char *quote = strchr(buf, '"');
        if (quote) {
            memmove(buf, quote + 1, strlen(buf));
            buf[strcspn(buf, "\"")] = '\0';
        }
        pclose(fp);
    } else {
        snprintf(buf, size, "Unknown OS");
    }
}

// 获取硬盘序列号（根分区）
void get_disk_serial(char *buf, size_t size) {
    FILE *fp = popen("lsblk -no SERIAL $(df / | grep '/dev/' | awk '{print $1}')", "r");
    if (fp) {
        fgets(buf, size, fp);
        buf[strcspn(buf, "\n")] = '\0';
        pclose(fp);
        if (strlen(buf) == 0) {
            snprintf(buf, size, "Unknown Disk Serial");
        }
    } else {
        snprintf(buf, size, "Unknown Disk Serial");
    }
}

// 守护进程初始化
void daemon_init() {
    // 1. 创建子进程，退出父进程
    pid_t pid = fork();
    if (pid < 0) {
        perror("fork failed");
        exit(EXIT_FAILURE);
    }
    if (pid > 0) {
        exit(EXIT_SUCCESS); // 父进程退出
    }

    // 2. 创建新会话，脱离控制终端
    if (setsid() < 0) {
        perror("setsid failed");
        exit(EXIT_FAILURE);
    }

    // 3. 忽略终端信号，设置文件权限掩码
    signal(SIGCHLD, SIG_IGN);
    signal(SIGHUP, SIG_IGN);
    umask(0);

    // 4. 切换根目录，关闭标准文件描述符
    chdir("/");
    for (int i = 0; i < 3; i++) {
        close(i);
    }
    // 重定向标准输出/错误到空设备
    freopen("/dev/null", "w", stdout);
    freopen("/dev/null", "w", stderr);
}

int main() {
    // 注册信号处理
    signal(SIGINT, sig_handler);
    signal(SIGTERM, sig_handler);

    // 初始化守护进程
    daemon_init();

    // 创建UNIX Domain Socket
    int server_fd = socket(AF_UNIX, SOCK_STREAM, 0);
    if (server_fd < 0) {
        perror("socket failed");
        exit(EXIT_FAILURE);
    }

    // 绑定套接字：先删除旧的套接字文件
    unlink(SOCKET_PATH);
    struct sockaddr_un server_addr;
    memset(&server_addr, 0, sizeof(server_addr));
    server_addr.sun_family = AF_UNIX;
    strncpy(server_addr.sun_path, SOCKET_PATH, sizeof(server_addr.sun_path) - 1);

    if (bind(server_fd, (struct sockaddr *)&server_addr, sizeof(server_addr)) < 0) {
        perror("bind failed");
        close(server_fd);
        exit(EXIT_FAILURE);
    }

    // 监听套接字
    if (listen(server_fd, 5) < 0) {
        perror("listen failed");
        close(server_fd);
        exit(EXIT_FAILURE);
    }

    // 设置套接字权限（允许客户端连接）
    chmod(SOCKET_PATH, 0777);

    printf("Daemon started, waiting for client...\n");

    while (server_running) {
        // 接受客户端连接（阻塞）
        int client_fd = accept(server_fd, NULL, NULL);
        if (client_fd < 0) {
            if (server_running) {
                perror("accept failed");
            }
            break;
        }

        // 循环采集信息（15秒周期）
        while (server_running) {
            char cpu[128], os[128], disk[128], time_str[64];
            get_cpu_info(cpu, sizeof(cpu));
            get_os_info(os, sizeof(os));
            get_disk_serial(disk, sizeof(disk));
            get_time(time_str, sizeof(time_str));

            // 拼接信息
            char info[512];
            snprintf(info, sizeof(info),
                     "【系统信息】\n"
                     "时间: %s\n"
                     "CPU型号: %s\n"
                     "OS版本: %s\n"
                     "硬盘序列号: %s\n"
                     "------------------------\n",
                     time_str, cpu, os, disk);

            // 发送信息给客户端
            ssize_t sent = send(client_fd, info, strlen(info), 0);
            if (sent <= 0) {
                // 客户端断开连接
                break;
            }

            sleep(INTERVAL); // 15秒间隔
        }

        close(client_fd);
    }

    // 清理资源
    close(server_fd);
    unlink(SOCKET_PATH);
    printf("Daemon exited.\n");
    return 0;
}