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

class TCPClient
{
public:
    TCPClient(const char *server_ip)
    {
        sock_fd = socket(AF_INET, SOCK_STREAM, 0);
        if (sock_fd < 0)
        {
            perror("sock failed");
            exit(EXIT_FAILURE);
        }
        memset(&serv_addr, 0, sizeof(serv_addr));
        serv_addr.sin_port = htons(PORT);
        serv_addr.sin_family = AF_INET;

        // 转化服务端IP
        if (inet_pton(AF_INET, server_ip, &serv_addr.sin_addr) <= 0)
        {
            perror("invalid address(ckeck IP format)");
            close(sock_fd);
            exit(EXIT_FAILURE);
        }

        if (connect(sock_fd, (struct sockaddr *)&serv_addr, sizeof(serv_addr)) < 0)
        {
            perror("connectron failed(ckeck server status/IP/poet)");
            close(sock_fd);
            exit(EXIT_FAILURE);
        }

        std::cout << "=== Connected to Server ===" << std::endl;
        std::cout << "Server IP: " << server_ip << std::endl;
        std::cout << "Server Port: " << PORT << std::endl;
    }

    ~TCPClient()
    {
        close(sock_fd);
        std::cout << "===Disconnected from Server===" << std::endl;
    }

    void conmmunicate(const char *message)
    {
        send(sock_fd, message, strlen(message), 0);
        std::cout << "\nSent to Server" << std::endl;

        char buffer[BUF_SIZE] = {0};
        ssize_t valread = read(sock_fd, buffer, sizeof(buffer));
        if (valread < 0)
        {
            perror("read failed");
        }
        else if (valread == 0)
        {
            std::cout << "Server disconneted(no response)" << std::endl;
        }
        else
        {
            std::cout << "Received fron server:" << buffer << "(bytes:" << valread << ")" << std::endl;
        }
    }

private:
    int sock_fd;
    struct sockaddr_in serv_addr; // 储存服务端地址信息
};

int main()
{
    TCPClient client("127.0.0.1");
    client.conmmunicate("Hello from Client");

    return 0;
}