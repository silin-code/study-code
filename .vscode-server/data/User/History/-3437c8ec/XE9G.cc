#include <iostream>
#include <string>
#include <cstdlib>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <netinet/in.h>
#include <unistd.h>
#include "InetAddr.hpp"

static void Usage(const std::string &name)
{
    std::cerr << "Usage:\n\t";
    std::cerr << name << "server_ip server_port" << std::endl;
}

int main(int argc, char *argv[])
{
    if (argc != 3)
    {
        Usage(argv[0]);
        exit(1);
    }
    std::string server_ip = argv[1];
    uint16_t server_port = atoi(argv[2]);
    int sockfd = -1;
    while (1)
    {
        int sockfd = socket(AF_INET, SOCK_STREAM, 0);
        if (sockfd < 0)
        {
            std::cerr << "socker error" << std::endl;
            exit(2);
            goto WAIT;
        }

        InetAddr serveraddress(server_port, server_ip);
        int n = connect(sockfd, (struct sockaddr *)serveraddress.getNetAddr(), serveraddress.Len());
        if (n < 0)
        {
            std::cerr << "connect to" << serveraddress.ToString() << "failed";
            exit(3);
        }
        std::cerr << "connect to" << serveraddress.ToString() << "sucess!";

    WAIT:
        sleep(1);
    }

    while (1)
    {
        int sockfd = socket(AF_INET, SOCK_STREAM, 0);

        std::string line;
        std::cout << "please enter#";
        std::getline(std::cin, line);

        write(sockfd, line.c_str(), line.size());

        char inbuffer[1024];
        ssize_t n = read(sockfd, inbuffer, sizeof(inbuffer));
    }
    return 0;
}