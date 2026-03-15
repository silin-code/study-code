#include <iostream>
#include <cstring>
#include <cstdlib>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <unistd.h>

#define PORT 8080
#define BUFFER_SIZE 1024

int main()
{
    int server_fd;
    struct sockaddr_in server_addr, client_addr;
    char buffer[BUFFER_SIZE] = {0};
    socklen_t addr_len = sizeof(client_addr);

    if ((server_fd=socket(AF_INET, SOCK_DGRAM, 0)) == 0)
    {
        perror("socket fail");
        exit(EXIT_FAILURE);
    }

    server_addr.sin_family = AF_INET;
    server_addr.sin_addr.s_addr = INADDR_ANY;
    server_addr.sin_port = htons(PORT);

    if (bind(server_fd, (struct sockaddr *)&server_addr, sizeof(server_addr)) < 0)
    {
        perror("bind fail");
        exit(EXIT_FAILURE);
    }

    std::cout << "UDP server is beginning,port:" << PORT << std::endl;

    while (true)
    {
        ssize_t recv_len = recvfrom(server_fd, buffer, BUFFER_SIZE, 0, (struct sockaddr *)&client_addr, &addr_len);

        if (recv_len < 0)
        {
            perror("recvfrom fail");
            exit(EXIT_FAILURE);
        }

        std::cout << "recve client message:" << buffer << std::endl;

        const char *response = "server have accepted your message";
        sendto(server_fd, response, strlen(response), 0, (struct sockaddr *)&client_addr, addr_len);
        std::cout << "send client response" << std::endl;

        memset(buffer, 0, BUFFER_SIZE);
    }
    return 0;
}