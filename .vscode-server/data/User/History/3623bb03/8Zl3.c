#include <curses.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>

// -------------------------- CPU 监控相关 --------------------------
typedef struct {
    unsigned long long user;
    unsigned long long nice;
    unsigned long long system;
    unsigned long long idle;
    unsigned long long iowait;
    unsigned long long irq;
    unsigned long long softirq;
} CpuStat;

// 读取 /proc/stat 获取 CPU 统计数据
void get_cpu_stat(CpuStat *stat) {
    FILE *fp = fopen("/proc/stat", "r");
    if (!fp) { perror("fopen /proc/stat failed"); exit(EXIT_FAILURE); }
    char buf[256];
    fgets(buf, sizeof(buf), fp);
    sscanf(buf, "cpu %llu %llu %llu %llu %llu %llu %llu",
           &stat->user, &stat->nice, &stat->system, &stat->idle,
           &stat->iowait, &stat->irq, &stat->softirq);
    fclose(fp);
}

// 计算 CPU 使用率（两次采样差值）
float calculate_cpu_usage(CpuStat *prev, CpuStat *curr) {
    unsigned long long prev_idle = prev->idle + prev->iowait;
    unsigned long long curr_idle = curr->idle + curr->iowait;
    unsigned long long prev_total = prev->user + prev->nice + prev->system + prev->idle + prev->iowait + prev->irq + prev->softirq;
    unsigned long long curr_total = curr->user + curr->nice + curr->system + curr->idle + curr->iowait + curr->irq + curr->softirq;
    unsigned long long total_diff = curr_total - prev_total;
    unsigned long long idle_diff = curr_idle - prev_idle;
    if (total_diff == 0) return 0.0f;
    return (1.0f - (float)idle_diff / total_diff) * 100.0f;
}

// -------------------------- 内存监控相关 --------------------------
typedef struct {
    unsigned long total;
    unsigned long free;
    unsigned long available;
    unsigned long buffers;
    unsigned long cached;
} MemInfo;

// 读取 /proc/meminfo 获取内存信息
void get_mem_info(MemInfo *info) {
    FILE *fp = fopen("/proc/meminfo", "r");
    if (!fp) { perror("fopen /proc/meminfo failed"); exit(EXIT_FAILURE); }
    char buf[256];
    while (fgets(buf, sizeof(buf), fp)) {
        if (sscanf(buf, "MemTotal: %lu kB", &info->total) == 1) continue;
        if (sscanf(buf, "MemFree: %lu kB", &info->free) == 1) continue;
        if (sscanf(buf, "MemAvailable: %lu kB", &info->available) == 1) continue;
        if (sscanf(buf, "Buffers: %lu kB", &info->buffers) == 1) continue;
        if (sscanf(buf, "Cached: %lu kB", &info->cached) == 1) continue;
    }
    fclose(fp);
}

// -------------------------- 网络监控相关 --------------------------
typedef struct {
    unsigned long long rx_bytes;
    unsigned long long tx_bytes;
} NetStat;

// 读取 /proc/net/dev 获取指定网卡的收发字节数
void get_net_stat(NetStat *stat, const char *iface) {
    FILE *fp = fopen("/proc/net/dev", "r");
    if (!fp) { perror("fopen /proc/net/dev failed"); exit(EXIT_FAILURE); }
    char buf[256];
    // 跳过前两行表头
    fgets(buf, sizeof(buf), fp);
    fgets(buf, sizeof(buf), fp);
    while (fgets(buf, sizeof(buf), fp)) {
        char name[32];
        unsigned long long rx, tx;
        if (sscanf(buf, " %s %llu %*u %*u %*u %*u %*u %*u %*u %llu", name, &rx, &tx) == 3) {
            if (strcmp(name, iface) == 0) {
                stat->rx_bytes = rx;
                stat->tx_bytes = tx;
                fclose(fp);
                return;
            }
        }
    }
    fclose(fp);
    fprintf(stderr, "Error: Network interface '%s' not found\n", iface);
    exit(EXIT_FAILURE);
}

// -------------------------- 主程序 & curses 界面 --------------------------
int main() {
    // 1. 初始化 curses 环境
    initscr();      // 初始化屏幕
    cbreak();       // 禁用行缓冲，直接读取字符
    noecho();       // 关闭输入回显
    keypad(stdscr, TRUE); // 启用功能键（如方向键）
    timeout(0);     // 非阻塞输入（getch 不等待）

    // 2. 获取终端尺寸，创建三个窗口
    int rows, cols;
    getmaxyx(stdscr, rows, cols);
    WINDOW *cpu_win = newwin(rows/2, cols/2, 0, 0);    // 左上：CPU 窗口
    WINDOW *mem_win = newwin(rows/2, cols/2, 0, cols/2); // 右上：内存窗口
    WINDOW *net_win = newwin(rows/2, cols, rows/2, 0);  // 下半部：网络窗口

    // 3. 初始化采样数据（先获取一次基准值）
    CpuStat prev_cpu, curr_cpu;
    get_cpu_stat(&prev_cpu);
    sleep(1); // 等待 1 秒后再采样，计算差值

    NetStat prev_net, curr_net;
    const char *net_iface = "eth0"; // 可改为 wlan0 等你的网卡名
    get_net_stat(&prev_net, net_iface);
    sleep(1);

    // 4. 主循环：实时更新监控数据
    while (1) {
        // --- 更新 CPU 数据 ---
        get_cpu_stat(&curr_cpu);
        float cpu_usage = calculate_cpu_usage(&prev_cpu, &curr_cpu);
        prev_cpu = curr_cpu;

        // --- 更新内存数据 ---
        MemInfo mem_info;
        get_mem_info(&mem_info);
        float mem_usage = 100.0f * (mem_info.total - mem_info.available) / mem_info.total;

        // --- 更新网络数据 ---
        get_net_stat(&curr_net, net_iface);
        unsigned long long rx_diff = curr_net.rx_bytes - prev_net.rx_bytes;
        unsigned long long tx_diff = curr_net.tx_bytes - prev_net.tx_bytes;
        float rx_rate = rx_diff / 1024.0f; // 转换为 KB/s
        float tx_rate = tx_diff / 1024.0f;
        prev_net = curr_net;

        // --- 刷新 CPU 窗口 ---
        werase(cpu_win);
        box(cpu_win, 0, 0); // 绘制边框
        mvwprintw(cpu_win, 1, 1, "=== CPU Usage ===");
        mvwprintw(cpu_win, 2, 1, "Usage: %.2f %%", cpu_usage);
        wrefresh(cpu_win);

        // --- 刷新内存窗口 ---
        werase(mem_win);
        box(mem_win, 0, 0);
        mvwprintw(mem_win, 1, 1, "=== Memory Info ===");
        mvwprintw(mem_win, 2, 1, "Usage: %.2f %%", mem_usage);
        mvwprintw(mem_win, 3, 1, "Total: %lu MB", mem_info.total / 1024);
        mvwprintw(mem_win, 4, 1, "Avail: %lu MB", mem_info.available / 1024);
        wrefresh(mem_win);

        // --- 刷新网络窗口 ---
        werase(net_win);
        box(net_win, 0, 0);
        mvwprintw(net_win, 1, 1, "=== Network (%s) ===", net_iface);
        mvwprintw(net_win, 2, 1, "RX: %.2f KB/s", rx_rate);
        mvwprintw(net_win, 3, 1, "TX: %.2f KB/s", tx_rate);
        wrefresh(net_win);

        // --- 检测退出按键（q/Q）---
        int ch = getch();
        if (ch == 'q' || ch == 'Q') break;

        sleep(1); // 每秒更新一次
    }

    // 5. 清理 curses 环境
    delwin(cpu_win);
    delwin(mem_win);
    delwin(net_win);
    endwin(); // 恢复终端正常模式

    return 0;
}