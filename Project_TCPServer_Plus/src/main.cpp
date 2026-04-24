//全局配置
#include "../config/Config.hpp"

//基础模块
#include "log/Logger.hpp"
#include "utils/Utils.hpp"

//网络模块
#include "net/TcpSocket.hpp"

//连接管理模块
#include "connection/Connection.hpp"

//Reactor + Select
#include "reactor/EventLoop.hpp"
#include "reactor/SelectLoop.hpp"

//线程文件
#include "thread/Thread.hpp"

#include <iostream>
#include <memory>
#include <vector>

// 全局管理连接
std::vector<std::shared_ptr<Connection>> g_connections;

// 初始化Select
SelectLoop loop;

void runloop()
{
    LOG_INFO("线程创建成功开始looprun");
    loop.run();
}


int main()
{
    LOG_INFO("===================================");
    LOG_INFO("    TCP 服务器启动(select版) %p", pthread_self());
    LOG_INFO("===================================");

    // 初始化监听Socket
    TcpSocket server;
    server.create();
    server.serveReuseAddr();
    server.bind(SERVER_PORT);
    server.listen(128);

    //初始化loop
    loop.init();

    Thread::createDetach(runloop);

    LOG_INFO("服务器等待客户端连接...");

    // 主线程事件循环
    while (true)
    {
        //持续处理
        sockaddr_in clientAddr;
        // 处理新连接
        int clientfd= server.accept(clientAddr);
        if(clientfd>0)
        {//只有clientfd是正确的我才往下走,说明有新链接来了
            //1:创建connection管理这个新链接
            std::shared_ptr<Connection> conn=std::make_shared<Connection>(clientfd);
            //先设置成阻塞模式
            server.setNonBlocking(clientfd);
            LOG_INFO("fd=%d的阻塞模式设置成功",clientfd);
            //加入全局列表
            g_connections.push_back(conn);
            LOG_INFO("fd=%d的加入g_connection成功",clientfd);
            //加入select
            loop.addConnection(conn.get());
            LOG_INFO("fd=%d的加入loop成功",clientfd);
        }
 
        //检查是否有connection已经是closed状态
        //从全局中删除,从loop中删除                           
        for (auto it = g_connections.begin(); it != g_connections.end(); )
        {
            // 判断当前连接是否已关闭
            if ((*it)->getStatus() == ConnStatus::CLOSED)
            {
                // 从列表删除 → 智能指针自动析构销毁Connection！
                 it = g_connections.erase(it);
            }
            else
            {
                  ++it;
            }
        }
    }

    return 0;
}