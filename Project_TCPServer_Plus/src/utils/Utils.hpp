#pragma once//Utils.hpp:工具类头文件

#include <string>
#include <sys/socket.h>
#include <unistd.h>
#include <netinet/in.h>
#include <arpa/inet.h>

//工具类
class Utils {
public:
    //错误检查函数
    static bool checkError(int ret,const std::string& msg);

    //IP转字符串
    static std::string getIPString(struct sockaddr_in addr);

    //安全关闭fd
    static void closefd(int fd);

    //端口字节序列转换
    static uint16_t htons(uint16_t port);
};