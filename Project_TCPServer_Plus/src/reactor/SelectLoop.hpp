#pragma once

//继承抽象事件循环基类
#include "EventLoop.hpp"
//连接管理类
#include "../connection/Connection.hpp"

//select头文件
#include <sys/select.h>
//STL容器存储连接
#include <vector>
#include <mutex>
#include <memory>

class SelectLoop : public EventLoop 
{
public:
    //重写三个核心接口
    bool init() override;
    bool addConnection(Connection* conn) override;
    void run() override;

private:
    //select核心:文件描述符集合
    fd_set m_readFds;
    /* fd_set是一个结构体，表示文件描述符集合
    fd_set内容：一个整数数组，每个元素表示一个文件描述符是否在集合中
    fd_set的操作：FD_ZERO清空集合，FD_SET添加文件描述符，FD_CLR删除文件描述符，FD_ISSET检查文件描述符是否在集合中*/

    //记录当前最大的文件描述符值，select需要这个值来确定监听范围
    int m_maxFd;

    //存储所有客户端的连接
    std::vector<Connection*> m_connections;
    //用指针存储连接对象，方便动态管理连接生命周期

    std::mutex mtx;
};
