#include "Utils.hpp"

//包含项目依赖
#include "../../config/Config.hpp"
#include "../log/Logger.hpp"


//1系统调用错误检查函数
//参数ret:系统调用返回值
//参数msg:错误信息
//打印失败信息和错误码，返回false；成功返回true
bool Utils::checkError(int ret,const std::string& msg)
{
    if(ret<0)//错误返回值小于0,打印错误信息和错误码
    {
        LOG_FATAL("%s 失败,错误码:%d",msg.c_str(),errno);
        return false;
    }
    return true;
}

//2IP转字符串
//参数addr:IP地址结构体
//返回值:IP地址字符串
std::string Utils::getIPString(struct sockaddr_in addr)
{
    char ip_buf[64]={0};
    inet_ntop(AF_INET,&addr.sin_addr,ip_buf,sizeof(ip_buf));
    return std::string(ip_buf);
}

//3安全关闭fd
void Utils::closefd(int fd)
{
    if(fd>0)
    {
        close(fd);//关闭clientfd->内核发送FIN，启动第三次挥手,发送FIN后fd就无效了，不能再使用了，所以不需要将fd置为-1
        LOG_INFO("关闭文件描述符:%d,触发四次挥手",fd);
    }
}

//4端口字节序列转换
uint16_t Utils::htons(uint16_t port)
{
    return ::htons(port);//htons函数将主机字节序转换为网络字节序
}