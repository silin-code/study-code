#ifndef __ECHOSERVER_HPP
#define __ECHOSERVER_HPP

#include <iostream>
#include <sys/socket.h>
#include <sys/types.h>
#include <string>
#include <cstdlib>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <strings.h>
#include <bits/types.h>
#include "Logger.hpp"

using namespace NS_LOG_MODULE;

const static int default_fd = -1;
const static int default_port = 8888;

enum
{
    SUCCWSS = 0,
    USAGE_ERR,
    SOCKET_ERR,
    BIND_ERR,
};

class UdpServer
{
public:
    UdpServer(uint16_t port, std::string &ip)
        : _port(port),
          _ip(ip),
          _sockfd(default_fd)
    {
    }
    ~UdpServer()
    {
        close(_sockfd);
    }

    void Init()
    {
        // First step:creat socket
        _sockfd = socket(AF_INET, SOCK_DGRAM, 0);
        if (_sockfd < 0)
        {
            LOG(LogLevel::FATAL) << "create socket error";
            exit(SOCKET_ERR);
        }
        LOG(LogLevel::INFO) << "creat socket success,socketfd:" << _sockfd;

        // Second step:Full internet message
        struct sockaddr_in local;
        bzero(&local, sizeof(local));
        local.sin_family = AF_INET;
        local.sin_addr.s_addr = inet_addr(_ip.c_str());
        local.sin_port = htons(_port);

        // Third step:bind
        int n = bind(_sockfd, (struct sockaddr *)&local, sizeof(local));
        if (n < 0)
        {
            LOG(LogLevel::FATAL) << "bind sokect error";
            exit(BIG_ENDIAN);
        }
        LOG(LogLevel::INFO) << "bind socket success,ip:" << _ip << ",port:" << _port;
    }

    void Start()
    {
        char inbuffer[1024];
        while (1)
        {
            struct sockaddr_in peer;
            socklen_t len = sizeof(peer);
            ssize_t n = recvfrom(_sockfd, inbuffer, sizeof(inbuffer) - 1, 0, (struct sockaddr *)&peer, &len);
            if (n > 0)
            {

                inbuffer[n] = 0;
                LOG(LogLevel::DEBUG) << "cilent say#" << inbuffer;
                std::string echo_string = "server echo#";
                echo_string += inbuffer;
                sendto(_sockfd, echo_string.c_str(), echo_string.size(), 0, (struct sockaddr *)&peer, len);
            }
            else
            {
                LOG(LogLevel::ERROR) << "recvfrom error";
            }
        }
    }

private:
    int _sockfd;
    std::string _ip;
    uint16_t _port;
};

#endif