#pragma once

#include <iostream>
#include <unistd.h>
#include <cstdlib>
#include <arpa/inet.h>
#include <netinet/in.h>
#include <sys/socket.h>
#include "InetAddr.hpp"
#include "Logger.hpp"

namespace NS_SOCKET
{
    using namespace NS_LOG_MODULE;

    static int gbacklog = 16;
    enum
    {
        OK = 0,
        SOCKET_ERR,
        BIND_ERR,
        LISTEN_ERR,
    };

    class Socket
    {
    public:
        ~Socket() {}

    public:
        virtual void CreatesocketOrDie() = 0;
        virtual void BindSocketOrDie(uint16_t port) = 0;
        virtual void ListenSocketOrDie() = 0;
        // virtual ssize_t Recv() = 0;
        // virtual void Send() = 0;

    public:
        virtual std::shared_ptr<Socket> Accept(InetAddr &addr) = 0;
        virtual int sockfd()=0;

        void BuildTcpSocketMethod(uint16_t port)
        {
            CreatesocketOrDie();
            BindSocketOrDie(port);
            ListenSocketOrDie();
        }

        // void BuildUdpSocket(uint16_t port)
        // {
        //     CreatesocketOrDie();
        //     BindSocketOrDie(port);
        // }
    };

    class TcpSocket : public Socket
    {
    public:
        TcpSocket(int sockfd) : _sockfd(sockfd) {}
        TcpSocket() : _sockfd(0) {}

        std::shared_ptr<Socket> Accept(InetAddr &clientaddr) override
        {
            struct sockaddr_in addr;
            socklen_t len = sizeof(addr);
            int sockfd = accept(_sockfd, CONV(&addr), &len);

            if (sockfd < 0)
            {
                LOG(LogLevel::WARNING) << "accept error";
                return nullptr;
            }
            clientaddr = addr; 
            return std::make_shared<TcpSocket>(sockfd);
        }
        void CreatesocketOrDie() override
        {
            _sockfd = socket(AF_INET, SOCK_STREAM, 0);
            if (_sockfd < 0)
            {
                LOG(LogLevel::FATAL) << "create socket error";
                exit(SOCKET_ERR);
            }
        }
        void BindSocketOrDie(uint16_t port) override
        {
            InetAddr addr(port);
            if (bind(_sockfd, addr.getNetAddr(), addr.Len()) != 0)
            {
                LOG(LogLevel::FATAL) << "bind socket error";
                exit(BIND_ERR);
            }
        }
        void ListenSocketOrDie() override
        {
            if (listen(_sockfd, gbacklog) != 0)
            {
                LOG(LogLevel::FATAL) << "listen socket error";
                exit(LISTEN_ERR);
            }
        }
        int sockfd() override
        {
            return _sockfd;
        }
        ~TcpSocket() {}

    private:
        int _sockfd;
    };

    // class UdpSocket : public Socket
    // {
    // };

}