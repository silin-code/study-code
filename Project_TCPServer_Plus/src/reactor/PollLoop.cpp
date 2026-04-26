#include "PollLoop.hpp"
#include "../../config/Config.hpp"
#include "../log/Logger.hpp"
#include "../connection/Connection.hpp"

#include <iostream>
#include <algorithm>

//事件初始化
bool PollLoop::init()
{
    m_pollFds.clear();
    m_connections.clear();
    LOG_INFO("SelectPool initialized successfully");
    return true;
}

//添加客户端到poll监听
bool PollLoop::addConnection(Connection* conn)
{
    if(conn==nullptr)
    {
        LOG_FATAL("Poll addconnection faild:null connetion pointer");
        return false;
    }

    std::lock_guard<std::mutex> lock(mtx);
    int clientfd= conn->getFd();

    /*struct pollfd {
    int fd;         // 待监控的文件描述符
    short events;   // 用户→内核：我要监控什么事件（输入）
    short revents;  // 内核→用户：实际发生了什么事件（输出）*/
    //浮躁pollfd结构体监听读事件
    pollfd pfd{};
    pfd.fd = clientfd;
    pfd.events = POLLIN;//监听客户端发送的数据

    m_pollFds.push_back(pfd);//加入核心结构体数组
    m_connections.push_back(conn);//加入连接管理队列
    LOG_INFO("PollLoop added connection fd=%d",clientfd);
    return true;
}

//Poll事件循环
void PollLoop::run()
{
    LOG_INFO("PollLoop event loop started %p",pthread_self());

    //保证只打印一次
    bool has_valid_event=true;
    while(true)
    {
        //线程安全拷贝
        std::vector<pollfd> tmpPollFds;
        std::vector<Connection*> tmpConns;

        {
            std::lock_guard<std::mutex> lock(mtx);
            tmpPollFds = m_pollFds;
            tmpConns = m_connections;
        }//锁立即释放，无死锁

        //poll系统调用
        //参数1:pollfd数组首地址
        //参数2:数组大小
        //参数3:超时时间(ms)
        //返回值,>0就绪时间总数;==0超时;==-1出错
        int eventnum= poll(tmpPollFds.data(),tmpPollFds.size(),5000);
        if(eventnum<=0)
        {
            continue;
        }
        else
        {
            if(has_valid_event)
            {
                LOG_INFO("Poll Success");
                has_valid_event=false;
            }
        }
        //处理就绪事件
        for(size_t i =0;i<tmpPollFds.size();i++)
        {
            //只处理就绪事件
            if(tmpPollFds[i].revents & POLLIN)
            {
                int ready_fd= tmpPollFds[i].fd;

                //找到对应链接
                for(auto conn:tmpConns)
                {
                    if(conn->getFd()==ready_fd)
                    {
                        //找到了
                        conn->readData();//处理客户端消息
                        break;
                    }
                }
            }
        }
    }
}