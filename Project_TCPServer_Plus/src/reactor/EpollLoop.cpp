#include "EpollLoop.hpp"
#include "../../config/Config.hpp"
#include "../log/Logger.hpp"
#include "../utils/Utils.hpp"
#include "../connection/Connection.hpp"

#include <iostream>
#include <string.h>
#include <algorithm>

EpollLoop::~EpollLoop()
{
    Utils::closefd(m_epfd);
}

//初始化Epoll
bool EpollLoop::init()
{
    //创建epoll实例(EPOLL_CLOEXEC进程自动关闭释放)
    m_epfd=epoll_create1(EPOLL_CLOEXEC);
    if(Utils::checkError(m_epfd,"Epoll create"))
    {
        LOG_INFO("Epoll created successfully");
        m_connections.clear();
        return true;
    }
    return false;
}

//添加客户端到epoll
bool EpollLoop::addConnection(Connection* conn)
{
    if(conn==nullptr||m_epfd<0)
    {
        LOG_FATAL("Epoll addConnection failed");
        return false;
    }

    std::lock_guard<std::mutex> lock(mtx);
    int clientfd=conn->getFd();

    struct epoll_event ev{};
    ev.events = EPOLLIN;//监听读事件
    ev.data.ptr = conn;//直接绑定连接对象,高效查找

    //EPOLL_CTL_ADD.添加fd到epoll
    if(epoll_ctl(m_epfd,EPOLL_CTL_ADD,clientfd,&ev)<0)
    {
        LOG_FATAL("epoll_ctl add failed,fd=%d",clientfd);
        return false;
    }

    m_connections.push_back(conn);
    LOG_INFO("EpollLoop added connection fd=%d", clientfd);
    return true;
}

//epoll事件循环
// 作用：持续监听所有客户端的IO事件（读事件），有消息就处理，没有就阻塞等待
// 运行在独立的子线程中，不阻塞主线程接收新连接
void EpollLoop::run()
{
    LOG_INFO("EpollLoop event loop startd:%p",pthread_self());
    // 定义事件数组：接收内核返回的【就绪客户端事件】，最大存储1024个
    struct epoll_event events[MAX_EVENTS];
    bool has_valid_event=true;
    while(true)
    {
        //线程安全拷贝
        std::vector<Connection*> tmpConns;
        {
            std::lock_guard<std::mutex> lock(mtx);
            tmpConns = m_connections;
        }
        
        // 参数说明：
        // m_epfd：epoll实例描述符
        // events：存储就绪事件的数组
        // MAX_EVENTS：最大监听事件数
        // 5000：超时时间5000ms=5秒，超时自动返回，不会永久卡死
        int eventnum = epoll_wait(m_epfd,events,MAX_EVENTS,5000);

        // eventnum <= 0:代表超时 或 系统调用出错，不处理任何逻辑
        if(eventnum<=0)
        {
            continue;
        }
        else
        {
            if(has_valid_event)
            {
                LOG_INFO("Epoll Success");
                has_valid_event=false;
            }
        }

        //处理事件
        for(int i=0;i<eventnum;i++)
        {
            //从epoll事件中,取出我们绑定的Connection连接对象（裸指针）
            Connection* conn =(Connection*)events[i].data.ptr;

            // ===================== 安全过滤：跳过已断开的客户端 =====================
            // 客户端断开后，状态为CLOSED，直接跳过，不处理，避免野指针/无限打印
            if(conn == nullptr || conn->getStatus() == ConnStatus::CLOSED)
            {
                continue;
            }

            //处理正确事件
            // EPOLLIN：内核通知，客户端有数据可以读取
            if(events[i].events & EPOLLIN)
            {
                conn->readData();
            }
        }
    }
}