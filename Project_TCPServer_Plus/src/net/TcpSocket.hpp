#pragma once 

//依赖头
#include "../../config/Config.hpp"
#include "../log/Logger.hpp"
#include "../utils/Utils.hpp"

//Linux网络头
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <fcntl.h>


#include <cstring>
//TCPSocket封装类
//作用：封装TCP套接字的创建、连接、发送、接收等操作，提供更高层次的接口，简化网络编程。
class TcpSocket{
public:
    //构造函数
    TcpSocket();
    //析构函数,自动关闭套接字
    ~TcpSocket();

    //Tcp服务器核心流程
    //1创建TCP套接字
    bool create();
    //2设置端口复用(解决TIME_WAIT问题)
    /*端口复用作用：允许多个套接字绑定到同一个端口，
    解决TIME_WAIT状态导致的端口占用问题，提高服务器的可用性和稳定性。*/
    void serveReuseAddr();
    //3绑定IP和端口port
    bool bind(uint16_t port);
    //4监听连接请求
    bool listen(int backlog = 128);//内核开始监听套接字，等待客户端连接请求，backlog参数指定连接请求队列的最大长度

    /*listen和accept中间
    客户端发起三次握手,内核完成SYN->SYN+ACK->ACK,应用层看不到,内核全权处理
    握手完成,accept才返回客户端fd*/

    //5接受客户端连接,返回新的套接字描述符
    int accept(sockaddr_in &clientAddr);
    /***阻塞等待客户端完成三次握手***,内核完成(SYN->SYN+ACK->ACK交互),
    握手成功后返回一个新的套接字描述符用于与客户端通信,同时将客户端地址信息存储在clientAddr中*/

public:
    //工具函数
    //设置套接字为非阻塞模式
    void setNonBlocking(int fd);
    //获取监听sokcet的fd
    int getListenFd() const;

private:
    int m_listenFd; //监听套接字描述符(服务端使用)
};