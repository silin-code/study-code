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

#include <iostream>
#include <memory>
#include <vector>

// 全局管理连接（单线程安全）
std::vector<std::shared_ptr<Connection>> g_connections;

int main()
{
    LOG_INFO("===================================");
    LOG_INFO("    TCP 服务器启动(select版)");
    LOG_INFO("===================================");

    // 初始化监听Socket
    TcpSocket server;
    server.create();
    server.serveReuseAddr();
    server.bind(SERVER_PORT);
    server.listen(128);

    // 获取监听FD（你原生的函数）
    int listen_fd = server.getListenFd();

    // 初始化Select
    SelectLoop loop;
    loop.init();

    LOG_INFO("服务器等待客户端连接...");

    // 单线程事件循环（修复所有权限/编译错误）
    while (true)
    {
        // 处理新连接
        sockaddr_in clientAddr;
        int clientfd = accept(listen_fd, (sockaddr*)&clientAddr, nullptr);
        if (clientfd > 0) {
            auto conn = std::make_shared<Connection>(clientfd);
            g_connections.push_back(conn);
            loop.addConnection(conn.get());
        }

        // 运行select事件循环
        loop.run();
    }

    return 0;
}