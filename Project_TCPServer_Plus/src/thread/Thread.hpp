#pragma once

//线程模块只依赖Log模块+C++11标准库
#include <thread>
#include "../log/Logger.hpp"


//封装线程类
//作用:简化std::thread使用
class Thread{
public:
    //禁用拷贝构造,赋值(线程不可复制)
    Thread(const Thread&) = delete;
    Thread& operator=(const Thread&) = delete;

public:

    //1创建分离线程(主线程退出,子线程自动结束)
    //适合IO处理,业务逻辑,不阻塞主进程
    template<typename Func,typename... Args>
    static void creatDetach(Func&& func,Args&&... args);

    //2创建等待线程(主线程等待子线程结束)
    //适合:只需要等待任务完成的场景
    template<typename Func,typename... Args>
    static void creatJoin(Func&& func,Args&&... args);
};

//模板函数实现(模板函数必须放在头文件中)
template<typename Func,typename... Args>
void Thread::creatDetach(Func&& func, Args&&... args) {
    //创建线程对象,传入函数和参数
    std::thread t(std::forward<Func>(func),std::forward<Args>(args)...);
    //分离线程(主线程退出,子线程自动结束)
    t.detach();
    LOG_DEBUG("创建分离线程成功");
}

template<typename Func,typename... Args>
void Thread::creatJoin(Func&& func, Args&&... args) {
    //创建线程对象,传入函数和参数
    std::thread t(std::forward<Func>(func),std::forward<Args>(args)...);
    //等待线程结束(主线程等待子线程完成)
    t.join();
    LOG_DEBUG("创建等待线程成功");
}