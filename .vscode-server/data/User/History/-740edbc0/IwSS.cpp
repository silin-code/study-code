#include <iostream>
#include <cstring>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <netinet/in.h>
#include <cstdio>

#define PORT 8080

int main()
{
    int sock = 0, valread;
    struct sockaddr_in serv_addr;
    char buffer[1024];
    const char *request = "Hello from TCP client";

    if ((sock = socket(AF_INET, SOCK_STREAM, 0) <= 0))
    {
        std::cout << "\nsocj=ket creation error" << std::endl;
        return -1;
    }

    serv_addr.sin_family = AF_INET;
    serv_addr.sin_port = htons(PORT);

    if (inet_pton(AF_INET, "127.0.0.1", &serv_addr.sin_addr) <= 0)
    {
        std::cout << "\nInvalid address/Address not supported\n";
        return -1;
    }

    if (connect(sock, (struct sockaddr *)&serv_addr, sizeof(serv_addr)) < 0)
    {
        std::cout << "\nconnection failed\n";
        return -1;
    }
    std::cout << "Connected to server" << std::endl;

    send(sock, request, strlen(request), 0);
    std::cout << "Request send:" << request << std::endl;

    valread = read(sock, buffer, 1024);
    std::cout << "Received from server:" << buffer << std::endl;

    close(sock);
    return 0;
}