#pragma once
#include "EventLoop.hpp"
#include "../connection/Connection.hpp"

#include <vector>
#include <poll.h>
#include <mutex>

class PollLoop : public EventLoop{
public:
    //接口和SelectPoll一样
    bool init() override;
    bool addConnection(Connection* conn) override;
    void run() override;

private:
    std::vector<pollfd> m_pollFds;//poll核心结构体数组
    std::vector<Connection*> m_connections;//储存连接对象的指针,方便管理
    std::mutex mtx;//线程锁
};