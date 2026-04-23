#ifndef CONFIG_HPP
#define CONFIG_HPP
#include "../src/log/LogLevel.hpp"

//选择IO模型
//IO_POLL = POLL_MODEL
//IO_EPOLL = EPOLL_MODEL

#define IO_MODEL_TYPE IO_POLL

//服务器配置
#define SERVER_PORT 8080 //默认端口
#define MAX_EVENT 1024 //最大监听事件数
#define BUFFER_SIZE 1024 //数据接收发送缓冲区大小
#define LOG_LEVEL LogLevel::DEBUG //日志级别

#endif