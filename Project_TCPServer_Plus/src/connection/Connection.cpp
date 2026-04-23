#include "Connection.hpp"

Connection::Connection(int fd) : m_clientfd(fd), 
    m_status(CONNECTED) {
    LOG_INFO("New connection established, fd: %d", m_clientfd);
}

Connection::~Connection() {
    closeConnection();
}

//读取客户端发送的数据
void Connection::readData() 
{
    if(m_status == CLOSED) return;
    //清空缓冲区
    memset(m_buffer,0,sizeof(m_buffer));
    
    //recv函数返回值:成功返回接收的字节数,连接关闭返回0,出错返回-1
    //参数说明:fd,缓冲区,缓冲区大小,标志
    //和read函数类似,但recv可以指定标志位,如MSG_DONTWAIT(非阻塞),MSG_PEEK(窥视数据)等
    //sizeof(m_buffer)-1是为了留出一个字节存放字符串结束符'\0'
    int len = recv(m_clientfd, m_buffer, sizeof(m_buffer)-1, 0);

    if(len<=0)
    {
        LOG_INFO("Connection closed by client, fd: %d", m_clientfd);
        closeConnection();
        return;
    }

    //打印接收到的数据
    LOG_INFO("Received data from client, fd: %d, data: %s", m_clientfd, m_buffer);  
    sendData("Message received: " + std::string(m_buffer));
}

//回复数据给客户端
void Connection::sendData(const std::string& msg)
{
    if(m_status == CLOSED) return;
    //send函数返回值:成功返回发送的字节数,出错返回-1
    //参数说明:fd,数据,数据长度,标志
    int len = send(m_clientfd, msg.c_str(), msg.size(), 0);
    if(len==-1)
    {
        LOG_FATAL("Failed to send data to client, fd: %d", m_clientfd);
        closeConnection();
    }
}

//关闭连接
void Connection::closeConnection()
{
    if(m_status == CLOSED) return;
    m_status = CLOSED;
    Utils::closefd(m_clientfd);
    LOG_INFO("Connection closed, fd: %d", m_clientfd);
}

//获取连接的fd
int Connection::getFd() const
{
    return m_clientfd;
}

//获取连接状态
ConnStatus Connection::getStatus() const
{
    return m_status;
}