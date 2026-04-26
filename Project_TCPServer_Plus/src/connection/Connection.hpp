#pragma once 

//依赖文件
#include "../log/Logger.hpp"
#include "../utils/Utils.hpp"
#include "../../config/Config.hpp"

#include <string>
#include <cstring>
#include <sys/socket.h>
#include <unordered_map>//汉译英扩展
#include <algorithm>


//连接状态:已连接,关闭
enum ConnStatus{
    CONNECTED,//已连接
    CLOSED//已关闭
};

extern std::unordered_map<std::string,std::string> g_dict;

//客户端连接管理类
class Connection{
public:
    //构造函数:传入客户端的fd
    explicit Connection(int fd);//explicit:禁止隐式转换
    //析构函数:关闭连接
    ~Connection();

    //核心功能
    void readData();//读取客户端发送的数据
    void sendData(const std::string& msg);//回复数据给客户端
    void closeConnection();//关闭连接

    //获取接口
    int getFd() const;//获取连接的fd
    ConnStatus getStatus() const;//获取连接状态

private:
    int m_clientfd;//客户端连接的fd
    ConnStatus m_status;//连接状态
    char m_buffer[BUFFER_SIZE];//数据缓冲区 

    //辅助:把字符串转成小写
    std::string toLower(const std::string& s)
    {
        std::string res=s;
        for(size_t i=0;i<s.size();i++)
        {
            res[i]=tolower(s[i]);
        }
        return res;
    }
};