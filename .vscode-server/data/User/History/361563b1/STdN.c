#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/socket.h>
#include <sys/un.h>
#include <signal.h>
#include <string.h>

#define SOCKET_PATH "/tmp/system_info_socket"

// 全局变量：用于Ctrl+Q处理
int client_running = 1;

// 信号处理函数：Ctrl+Q触发退出
void sig_handler(int sig) {
    if (sig == SIGQUIT) {
        client_running = 0;
        printf("\nClient exited (Ctrl+Q pressed).\n");
    }
}

int main() {
    // 注册Ctrl+Q信号处理
    signal(SIGQUIT, sig_handler);

    // 创建客户端套接字
    int client_fd = socket(AF_UNIX, SOCK_STREAM, 0);
    if (client_fd < 0) {
        perror("socket failed");
        exit(EXIT_FAILURE);
    }

    // 连接守护进程
    struct sockaddr_un server_addr;
    memset(&server_addr, 0, sizeof(server_addr));
    server_addr.sun_family = AF_UNIX;
    strncpy(server_addr.sun_path, SOCKET_PATH, sizeof(server_addr.sun_path) - 1);

    if (connect(client_fd, (struct sockaddr *)&server_addr, sizeof(server_addr)) < 0) {
        perror("connect failed (daemon not running?)");
        close(client_fd);
        exit(EXIT_FAILURE);
    }

    printf("Connected to daemon. Press Ctrl+Q to exit.\n");
    printf("------------------------\n");

    // 接收守护进程发送的信息
    char buf[1024];
    while (client_running) {
        ssize_t recv_len = recv(client_fd, buf, sizeof(buf) - 1, 0);
        if (recv_len <= 0) {
            printf("\nDisconnected from daemon.\n");
            break;
        }
        buf[recv_len] = '\0';
        printf("%s", buf);
    }

    // 清理资源
    close(client_fd);
    return 0;
}