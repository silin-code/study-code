#include "Logger.hpp"
#include <time.h>
#include <stdio.h>

//动态修改日志级别
void Logger::setLogLevel(LogLevel level)
{
    m_level = level;
}

//获取当前时间字符串
std::string Logger::GetCurrentTime()
{
    time_t now =time(nullptr);
    tm* t = localtime(&now);

    char buf[64] ={0};
    snprintf(buf,sizeof(buf),"%04d-%02d-%02d %02d:%02d:%02d",
             t->tm_year + 1900,t->tm_mon + 1,t->tm_mday,
             t->tm_hour,t->tm_min,t->tm_sec);

    return std::string(buf);
}

//日志格式化输出
//参数说明:
//level:日志级别
//file:日志来源文件名
//line:日志来源行号
//format:日志内容格式字符串，类似printf的格式
//...:可变参数列表，传入日志内容的具体值
void Logger::log(LogLevel level,const char* file,int line,const char* format,...)
{
    //级别过滤:低于当前级别的日志不输出
    if(level <m_level) return;

    //加锁保证多线程环境下日志输出不混乱,RAII自动释放锁
    std::lock_guard<std::mutex> lock(mtx);

    //拼接日志内容
    std::string time_str=GetCurrentTime();
    std::string level_str = levelToString(level);//函数在LogLevel.hpp中定义，级别转字符串
    std::string prefix = "[" + time_str + "][" + level_str + "][" + file + ":" + std::to_string(line) + "] ";
    
    //处理可变参数，格式化日志内容
    char buf[1024] = {0};
    va_list args;
    va_start(args,format);
    vsnprintf(buf,sizeof(buf),format,args);
    va_end(args);

    //最终日志
    std::string log_msg = prefix + buf;
    //输出到控制台
    std::cout << log_msg << std::endl;
}