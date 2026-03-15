#include <iostream>
#include <netinet/in.h>
#include <string>
#include <cstring>
#include <cstdlib>
#include <arpa/inet.h>
#include <cstdio>
#include <unistd.h>
#include <string>
#include <signal.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <sys/wait.h>
#include <memory>
#include <pthread.h>

#define PORT 8080
#define BUF_SIZE 1024
#define SS struct sockaddr

void *handle_client(void *args)
{
    int *p_conn_fd = (int *)args;
    int conn_fd = *p_conn_fd;

    delete p_conn_fd;

    char buffer[BUF_SIZE] = {0};

    ssize_t valread = recv(conn_fd, buffer, sizeof(buffer), 0);
    if (valread < 0)
    {
        perror("receive failed");
    }
    else if (valread < 0)
    {
        std::cout << "thread:" << pthread_self() << "receive:" << buffer << std::endl;
    }
    else
    {
        std::cout << "thread:" << pthread_self() << "receive:" << buffer << std::endl;
        std::string response = "thread has received message";
        send(conn_fd, response.c_str(), strlen(response.c_str()), 0);
        std::cout << "server has responsed client" << std::endl;
    }
    close(conn_fd);
    pthread_exit(nullptr);
}

class TCPServer
{
public:
    TCPServer()
    {
        // 创建监听套接字
        listen_fd = socket(AF_INET, SOCK_STREAM, 0);
        if (listen_fd < 0)
        {
            perror("socket failed");
            exit(EXIT_FAILURE);
        }

        int opt = 1;
        if (setsockopt(listen_fd, SOL_SOCKET, SO_REUSEADDR | SO_REUSEPORT, &opt, sizeof(opt)))
        {
            perror("setsockopt failed");
            close(listen_fd);
            exit(EXIT_FAILURE);
        }

        // 初始化服务端地址结构体
        memset(&serv_addr, 0, sizeof(serv_addr));
        serv_addr.sin_family = AF_INET;
        serv_addr.sin_port = htons(PORT);
        serv_addr.sin_addr.s_addr = INADDR_ANY; // 服务端需要监听全部网卡的IP

        // 绑定套接字到IP+端口
        if (bind(listen_fd, (SS *)&serv_addr, sizeof(serv_addr)) < 0)
        {
            perror("bind failed");
            close(listen_fd);
            exit(EXIT_FAILURE);
        }

        // 启动监听(让套接字从主动变成被动，被动等待客户端连接)
        if (listen(listen_fd, 3) < 0)
        {
            perror("listen failed");
            close(listen_fd);
            exit(EXIT_FAILURE);
        }

        std::cout << "===TCP Server Initialized===" << std::endl;
        std::cout << "Listening on port:" << PORT << std::endl;
        std::cout << "Waiting for client connections..." << std::endl;
    }

    ~TCPServer()
    {
        close(listen_fd);
        std::cout << "Server Close" << std::endl;
    }

    // 阻塞等待连接->创建子进程->子进程处理通信->父进程继续监听
    void run()
    {
        // 创建客户端地址结构体，accpet成功后自动填充
        struct sockaddr_in cli_addr;
        // 必须传递指针，accpet会修改成真实长度
        socklen_t cli_len = sizeof(cli_addr);
        // 死循环，持续接受连接
        while (true)
        {
            // 阻塞监听，监听到listen_fd的消息，自动填充客户端地址到服务端创建的结构体，成功返回新的套接字，方便通信
            // 没有客户端连接会一直阻塞
            int conn_fd = accept(listen_fd, (SS *)&cli_addr, &cli_len);
            if (conn_fd < 0)
            {
                perror("accept failed");
                continue;
            }

            std::cout << "===New Client Connect===" << std::endl;
            std::cout << "Client IP:" << inet_ntoa(cli_addr.sin_addr) << std::endl;
            std::cout << "Client Port:" << ntohs(cli_addr.sin_port) << std::endl;

            // 接收完成，开始创建线程
            auto p_conn_fd = std::make_unique<int>(conn_fd);
            pthread_t tid;

            if (pthread_create(&tid, nullptr, handle_client, p_conn_fd.release()) != 0)
            {
                perror("thread create filed");
                close(conn_fd);
                continue;
            }
            pthread_detach(tid);
            std::cout << "create thread:" << tid << "handle client" << std::endl;
        }
    }

private:
    int listen_fd;                // 监听套接字
    struct sockaddr_in serv_addr; // 服务端地址结构体
};

int main()
{
    TCPServer server;
    server.run();
    return 0;
}