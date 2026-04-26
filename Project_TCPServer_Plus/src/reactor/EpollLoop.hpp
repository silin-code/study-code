#pragma once
#include "EventLoop.hpp"
#include "../connection/Connection.hpp"

#include <vector>
#include <sys/epoll.h>
#include <mutex>
#include <unistd.h>

class EpollLoop : public EventLoop{
public:
    bool init() override;
    bool addConnection(Connection* conn) override;
    void run() override;

    ~EpollLoop();
private:
    int m_epfd;//epoll实力文件描述符
    std::vector<Connection*> m_connections;
    std::mutex mtx;
    //epoll最大监听数
    static const int MAX_EVENTS = 1024;
};