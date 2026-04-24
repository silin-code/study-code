#include "Connection.hpp"

std::unordered_map<std::string,std::string> g_dict=
{
    {"hello", "你好"},
    {"world", "世界"},
    {"server", "服务器"},
    {"exit", "已断开连接"},
    {"bye", "再见"}
};

Connection::Connection(int fd) : m_clientfd(fd), 
    m_status(CONNECTED) {
        //创建成功自动打印日志
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
        closeConnection();//自动断开
        return;
    }
    //1清理换行和空格
    std::string input(m_buffer);
    input.erase(input.find_last_not_of(" \n\r\t")+1);
    std::string key=toLower(input);
    
    //2:汉译英
    std::string response;
    if(g_dict.count(key))
    {
        response=g_dict[key];
        LOG_INFO("翻译成功：%s → %s (fd=%d)", input.c_str(), response.c_str(), m_clientfd);
    }
    else
    {
        response = "未找到该词条，请重试";
        LOG_INFO("未找到词条：%s (fd=%d)", input.c_str(), m_clientfd);
    }

    //3回复客户端
    sendData(response+'\n');
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