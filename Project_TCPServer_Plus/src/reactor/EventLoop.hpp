#pragma once

//依赖
#include "../../config/Config.hpp"
#include "../log/Logger.hpp"
#include "../utils/Utils.hpp"

//前置声明Connection类
class Connection;

// ==============================================
// 抽象事件循环基类
// 功能：定义 select/poll/epoll 必须实现的统一接口
// 特点：不能直接创建对象，只能被继承
// ==============================================

class EventLoop {
public:
    // 析构函数
    virtual ~EventLoop() = default;
    
    //三大核心接口
    //1初始化:select/poll/epoll的初始化
    virtual bool init() = 0;

    ///2添加客户端连接到监听集合
    virtual bool addConnection(Connection* conn) = 0;

    //3事件循环:不断监听事件，处理事件、
    virtual void run() = 0;
};