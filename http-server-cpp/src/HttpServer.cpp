#include "../include/HttpServer.h"
#include <iostream>
#include <fstream>
#include <sstream>
#include <cstring>
#include <unistd.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>

// 缓冲区大小(读取浏览器请求)
#define BUFFER_SIZE 4096

// 构造函数，初始化端口
HttpServer::HttpServer(int port) : port(port), server_fd(-1) {}

// 获取服务器公网IP
std::string HttpServer::getPublicIP()
{
    char buffer[128] = {0};
    // 调用Linux命令查看公网IP
    FILE *fp = popen("curl -s ifconfig.me", "r");
    if (fp)
    {
        fgets(buffer, sizeof(buffer), fp);
        pclose(fp);
    }

    if (strlen(buffer) == 0)
    {
        return "请手动查询公网IP";
    }
    return std::string(buffer);
}

// 1/初始化socket Tcp服务器(HTTP基于TCP)
bool HttpServer::initSocket()
{
    // 1:创建socket文件描述符
    server_fd = socket(AF_INET, SOCK_STREAM, 0);
    if (server_fd == -1)
    {
        std::cerr << "Socket 创建失败" << std::endl;
        return false;
    }

    // 2:设置端口复用(防止重启报错)
    int opt = 1;
    setsockopt(server_fd, SOL_SOCKET, SO_REUSEADDR | SO_REUSEPORT, &opt, sizeof(opt));

    // 3:绑定服务器IP和端口
    struct sockaddr_in address;
    address.sin_family = AF_INET;
    address.sin_addr.s_addr = INADDR_ANY;
    address.sin_port = htons(port);

    if (bind(server_fd, (struct sockaddr *)&address, sizeof(address)) < 0)
    {
        std::cerr << "端口绑定失败" << std::endl;
        return false;
    }

    // 4:监听端口(最大连接数5)
    if (listen(server_fd, 5) < 0)
    {
        std::cerr << "监听失败" << std::endl;
        return false;
    }
    // 自动打印公网IP
    std::string ip = getPublicIP();
    std::cout<<"================================"<<std::endl;
    std::cout << "服务器启动成功，监听窗口:" << port << std::endl;
    std::cout << "你的服务器公网IP:" << ip << std::endl;
    std::cout << "浏览器访问地址:http://:" << ip << ":" << port << std::endl;
    std::cout<<"================================"<<std::endl;
    return true;
}

// 前端处理函数
std::string HttpServer::readFrontEndFile()
{
    std::ifstream file("web/index.html");
    if (!file.is_open())
    {
        return "<h1>前端文件未找到!</h1>";
    }

    std::stringstream buffer;
    buffer << file.rdbuf();
    file.close();
    return buffer.str();
}

// 处理客户端请求
void HttpServer::handleClient(int client_fd)
{
    char buffer[BUFFER_SIZE] = {0};
    read(client_fd, buffer, sizeof(buffer));

    // 打印请求
    std::cout << "\n====浏览器HTTP请求====\n"
              << buffer << "\n=====================\n";

    // 1:读取独立前端页面
    std::string html = readFrontEndFile();
    // 2:标准HTTP响应协议
    std::string response =
        "HTTP/1.1 200 OK\r\n"
        "Content-Type: text/html; charset=utf-8\r\n"
        "Connection: close\r\n"
        "\r\n" +
        html;

    // 发送给浏览器
    write(client_fd, response.c_str(), response.size());
    close(client_fd);
}

// 启动服务器
void HttpServer::start()
{
    if (!initSocket())
        return;
    struct sockaddr_in client_addr;
    socklen_t addr_len = sizeof(client_addr);

    while (true)
    {
        int client_fd = accept(server_fd, (struct sockaddr *)&client_addr, &addr_len);
        std::cout << "新链接:" << inet_ntoa(client_addr.sin_addr) << std::endl;
        handleClient(client_fd);
    }
}

// 关闭服务器
void HttpServer::stop()
{
    if (server_fd != -1)
        close(server_fd);
}