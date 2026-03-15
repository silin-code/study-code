#include <iostream>
#include <cstring>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <unistd.h>

#define PORT 8080
#define BUFFER_SIZE 1024

int main()
{
    int client_fd;
    struct sockaddr_in server_addr;
    char buffer[BUFFER_SIZE] = {0};
    const char *message = "hi UDP server";

    if (client_fd = socket(AF_INET, SOCK_DGRAM, 0) == 0)
    {
        perror("sock fail");
        exit(EXIT_FAILURE);
    }

    server_addr.sin_family = AF_INET;
    server_addr.sin_port = htons(PORT);
    if (inet_pton(AF_INET, "127.0.0.1", &server_addr.sin_addr) <= 0)
    {
        perror("inet_port changing fail");
        exit(EXIT_FAILURE);
    }

    sendto(client_fd, message, strlen(message), 0, (struct sockaddr *)&server_addr, sizeof(server_addr));
    std::cout << "已经向客户端发送消息:" << message << std::endl;

    ssize_t recv_len = recvfrom(client_fd, buffer, BUFFER_SIZE, 0, nullptr, nullptr);
    if (recv_len < 0)
    {
        perror("recvfrom fail");
        exit(EXIT_FAILURE);
    }

    std::cout << "收到服务器响应:" << buffer << std::endl;
    close(client_fd);
    return 0;
}