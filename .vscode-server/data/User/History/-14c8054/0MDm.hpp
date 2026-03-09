#pragma once

#include <iostream>
#include <cstring>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <netinet/in.h>
#include "Logger.hpp"
#include "InetAddr.hpp"

using namespace NS_LOG_MODULE;

static const int gbacklog = 16;
static const uint16_t gport = 8888;

enum
{
    SUCCESS,
    SOCKET_ERR,
    BIND_ERR,
    LISTEN_ERR,
};

class TcpServer
{
public:
    void Init()
    {
        _listensockfd = socket(AF_INET, SOCK_STREAM, 0);
        if (_listensockfd < 0)
        {
            LOG(LogLevel::FATAL) << "creat socket fail";
            exit(SOCKET_ERR);
        }
        LOG(LogLevel::DEBUG) << "socket creat success:" << _listensockfd;

        struct sockaddr_in local;
        memset(&local, 0, sizeof(local));
        local.sin_family = AF_INET;
        local.sin_port = htons(_port);
        local.sin_addr.s_addr = INADDR_ANY;

        int n = bind(_listensockfd, (struct sockaddr *)&local, sizeof(local));
        if (n < 0)
        {
            LOG(LogLevel::FATAL) << "bind fail";
            exit(BIND_ERR);
        }
        LOG(LogLevel::DEBUG) << "bind success";

        n = listen(_listensockfd, gbacklog);
        if (n < 0)
        {
            LOG(LogLevel::FATAL) << "listen error";
            exit(LISTEN_ERR);
        }
        LOG(LogLevel::DEBUG) << "listen success";
    }

    void serviceIO(int sockfd, InetAddr &address)
    {
        char inbuffer[1024];
        while (1)
        {
            ssize_t n = read(sockfd, inbuffer, sizeof(inbuffer) - 1);
            if (n > 0)
            {
                inbuffer[n] = 0;
                LOG(LogLevel::INFO) << address.ToString() << "say#" << inbuffer;

                std::string echo_string = "server echo#";
                echo_string += inbuffer;
            }
            else if (n == 0)
            {
                LOG(LogLevel::INFO) <<"client quit,address:" << address.ToString();
                break;
            }
            else
            {
                LOG(LogLevel::ERROR) << "client read error,address:" << address.ToString();
                break;
            }
        }

        void Start()
        {
            while (1)
            {
                struct sockaddr_in clientaddr;
                socklen_t len = sizeof(clientaddr);

                int sockfd = accept(_listensockfd, (struct sockaddr *)&clientaddr, &len);
                if (sockfd < 0)
                {
                    LOG(LogLevel::WARING) << "accept error";
                    continue;
                }
                LOG(LogLevel::DEBUG) << "accept  success,sockfd:" << sockfd;

                close(sockfd);
            }
        }

        TcpServer()
        {
        }
        ~TcpServer()
        {
        }

    private:
        uint16_t _port;
        int _listensockfd;
    };
}