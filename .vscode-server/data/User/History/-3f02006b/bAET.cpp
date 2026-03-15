#include <iostream>
#include <netinet/in.h>
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

#define PORT 8080
#define BUF_SIZE 1024
#define SS struct sockaddr

void sig_child(int sig)
{
    (void)sig;
    while (waitpid(-1, nullptr, WNOHANG) > 0)
        ;
}

class TCPServer
{
public:
    TCPServer()
    {
        //创建监听套接字
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

        //初始化服务端地址结构体
        memset(&serv_addr, 0, sizeof(serv_addr));
        serv_addr.sin_family = AF_INET;
        serv_addr.sin_port = htons(PORT);
        serv_addr.sin_addr.s_addr = INADDR_ANY;//服务端需要监听全部网卡的IP

        //绑定套接字到IP+端口
        if (bind(listen_fd, (SS *)&serv_addr, sizeof(serv_addr)) < 0)
        {
            perror("bind failed");
            close(listen_fd);
            exit(EXIT_FAILURE);
        }

        //启动监听(让套接字从主动变成被动，被动等待客户端连接)
        if (listen(listen_fd, 3) < 5)
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

    //阻塞等待连接->创建子进程->子进程处理通信->父进程继续监听
    void run()
    {
        //创建客户端地址结构体，accpet成功后自动填充
        struct sockaddr_in cli_addr;
        //必须传递指针，accpet会修改成真实长度
        socklen_t cli_len = sizeof(cli_addr);
        //死循环，持续接受连接
        while (true)
        {
            //阻塞监听，监听到listen_fd的消息，自动填充客户端地址到服务端创建的结构体，成功返回新的套接字，方便通信
            //没有客户端连接会一直阻塞
            int conn_fd = accept(listen_fd, (SS *)&cli_addr, &cli_len);
            if (conn_fd < 0)
            {
                perror("accept failed");
                continue;
            }

            std::cout << "===New Client Connect===" << std::endl;
            std::cout << "Client IP:" << inet_ntoa(cli_addr.sin_addr) << std::endl;
            std::cout << "Client Port:" << ntohs(cli_addr.sin_port) << std::endl;

            // 接收完成，开始创建进程

            pid_t pid = fork();
            if (pid < 0)
            {
                perror("fork failed");
                close(listen_fd);
                continue;
            }
            else if (pid == 0)
            {
                // 子进程只负责处理通信，不负责监听
                // listen_fd是监听套接字
                close(listen_fd);
                char buffer[BUF_SIZE] = {0};
                //返回实际接受到的字节数
                ssize_t valread = recv(conn_fd, buffer, BUF_SIZE, 0);
                if (valread < 0)
                {
                    perror("recv failed");
                }
                else if (valread == 0)
                {
                    std::cout << "Client" << inet_ntoa(cli_addr.sin_addr) << ":" << ntohs(cli_addr.sin_port)
                              << "disconnected(no data received)" << std::endl;
                }
                else
                {
                    std::cout << "Received from client:" << buffer << "(butes:" << valread << ")" << std::endl;

                    //给服务端回信
                    const char *response = "Server received your message!";
                    send(conn_fd, response, strlen(response), 0);
                    std::cout << "Sent response to client" << response << std::endl;
                }

                close(conn_fd);
                std::cout << "===Client Connection Closed(Child Process)===" << std::endl;
                exit(EXIT_SUCCESS);
            }
            else
            {
                close(conn_fd);
                std::cout << "child process created(PID:" << pid << "),continuingh to listen..." << std::endl;
            }
        }
    }

private:
    int listen_fd;//监听套接字
    struct sockaddr_in serv_addr;//服务端地址结构体
};

int main()
{
    signal(SIGCHLD,sig_child);

    TCPServer server;
    server.run();
    return 0;
}