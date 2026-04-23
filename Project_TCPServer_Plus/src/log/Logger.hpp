#pragma once//Logger.hpp:日志类头文件
#include <iostream>
#include <ctime>
#include <mutex>
#include <cstdarg>//处理可变参数
#include "LogLevel.hpp"
#include "../../config/Config.hpp"

//日志类(单例模式:全局只有一个对象)

class Logger{
private:
    LogLevel m_level;//当前日志级别
    std::mutex mtx;//日志输出锁，保证多线程环境下日志输出不混乱
private:
    //私有构造函数:禁止外部创建对象
    Logger(){
    m_level = LOG_LEVEL;//从配置文件读取日志级别
    }

    //获取当前时间字符串
    std::string GetCurrentTime();
public:
    //获取单例实例(线程安全)
    static Logger* getInstance(){//作用:提供全局访问点，获取日志对象实例
        static Logger instance;//局部静态变量，第一次调用时创建，线程安全
        return &instance;
    }

    //打印日志核心函数
    void log(LogLevel level,const char* file,int line,const char* format,...);

    //动态修改日志级别
    void setLogLevel(LogLevel level);

    //禁用拷贝构造和赋值
    Logger(const Logger&)=delete;
    Logger& operator=(const Logger&) =delete;
};

//日志宏(简化调用)
//解释:使用宏定义不同级别的日志函数，调用时自动传入文件名和行号，方便调试和定位问题
//参数说明:
//format:日志内容格式字符串，类似printf的格式
//...:可变参数列表，传入日志内容的具体值
//Logger::getInstance()->log:获取日志对象实例并调用log函数输出日志
//->指的是调用Logger的log函数，传入日志级别、文件名、行号和格式化的日志内容
#define LOG_DEBUG(format,...) Logger::getInstance()->log(LogLevel::DEBUG,__FILE__,__LINE__,format,##__VA_ARGS__)
#define LOG_INFO(format,...) Logger::getInstance()->log(LogLevel::INFO,__FILE__,__LINE__,format,##__VA_ARGS__)
#define LOG_WARN(format,...) Logger::getInstance()->log(LogLevel::WARN,__FILE__,__LINE__,format,##__VA_ARGS__)
#define LOG_FATAL(format,...) Logger::getInstance()->log(LogLevel::FATAL,__FILE__,__LINE__,format,##__VA_ARGS__)
