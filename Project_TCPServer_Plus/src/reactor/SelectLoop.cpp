#include "SelectLoop.hpp"

//包含依赖路径
#include "../../config/Config.hpp"
#include "../log/Logger.hpp"
#include "../utils/Utils.hpp"
#include "../connection/Connection.hpp"


//初始化select事件循环
//功能:初始化SelectLoop对象，准备好监听文件描述符集合
bool SelectLoop::init() {
    //1:使用FD_ZERO初始化文件描述符集合，清空集合
    //作用：确保集合中没有任何文件描述符，准备好添加新的连接
    FD_ZERO(&m_readFds);
    //&m_readFds是fd_set类型的指针，传递给FD_ZERO函数以初始化文件描述符集合

    //2:初始化最大文件描述符值为0，表示当前没有任何连接
    //作用：在添加连接时会更新这个值，select需要知道监听的文件描述符范围
    m_maxFd = 0;

    //3:日志记录初始化成功
    LOG_INFO("SelectLoop initialized successfully");

    return true;
}

//添加连接到select事件循环
//函数功能:将新的客户端连接添加到SelectLoop的监听集合中，以便在事件循环中能够监听这个连接上的事件
//参数说明:
//conn:指向Connection对象的指针，表示要添加的客户端连接
//返回值:如果添加成功返回true，否则返回false
bool SelectLoop::addConnection(Connection* conn) {
    //1合法性检查:判断连接指针是否为空,空指针直接返回
    if(conn == nullptr) {
        LOG_FATAL("Select addConnection failed: null connection pointer");
        return false;
    }

    std::lock_guard<std::mutex> lock(mtx);
    //2获取连接的文件描述符
    int clientfd = conn->getFd();

    //3使用FD_SET宏将连接的文件描述符添加到监听集合中
    //作用：告诉select函数需要监听这个文件描述符上的事件
    std::cout << clientfd << " "<< std::endl;
    FD_SET(clientfd, &m_readFds);

    //4更新最大文件描述符值，如果新连接的文件描述符大于当前最大值，则更新
    if(clientfd > m_maxFd) {
        m_maxFd = clientfd;
    }   

    //5将连接对象加入vector容器
    //作用：方便后续管理连接对象，比如在事件循环中处理事件时可以遍历这个容器
    m_connections.push_back(conn);

    //6日志记录添加连接成功
    LOG_INFO("SelectLoop added connection fd=%d", clientfd);
    return true;
}

//select事件循环
//功能:不断监听文件描述符集合中的事件，并处理这些事件
void SelectLoop::run() {
    //1日志记录事件循环开始
    LOG_INFO("SelectLoop event loop started %p",pthread_self());

    //2事件循环:使用while(true)实现死循环，不断监听事件
    while(true) {
        /*核心注意
        select调用会修改传入的文件描述符集合,
        所以每次调用前需要重新设置监听集合,
        这里我们使用一个临时变量tempFds来保存当前的监听集合,
        每次调用select前都将m_readFds复制到tempFds中,
        确保m_readFds保持不变，正确监听所有连接的事件
        */
        fd_set tempFds = m_readFds;
        int currentMaxfd;
        std::vector<Connection*> tempConns;

        //枷锁拷贝
        {
        std::lock_guard<std::mutex> lock(mtx);
        tempFds=m_readFds;
        currentMaxfd=m_maxFd;
        tempConns = m_connections;
        }
        //select调用说明
        //参数说明:
        // 参数1：最大文件描述符 + 1（select规定的固定用法）
        // 参数2：监听读事件的集合（我们只关心客户端发数据）
        // 参数3：监听写事件的集合（nullptr-不监听）
        // 参数4：监听异常事件的集合（nullptr-不监听）
        // 参数5：超时时间（nullptr-永久阻塞，直到有事件触发）
        // 返回值：就绪的文件描述符数量

        struct timeval tv;
        tv.tv_sec = 5;
        tv.tv_usec = 0;

        int eventnum = select(m_maxFd + 1, //最大文件描述符值 + 1
            &tempFds, //监听读事件的集合
            nullptr, //不监听写事件
            nullptr, //不监听异常事件
            &tv);//永久阻塞，直到有事件触发
        
        //std::cout << "select 函数返回就绪数量是： " << eventnum << std::endl; 

        //3：检查select调用结果
        if(eventnum<=0) continue;
            //4：遍历所有连接，检查哪些连接的文件描述符在就绪集合中
            for(Connection* conn: m_connections)
            {
                if(conn==nullptr||conn->getStatus()==ConnStatus::CLOSED)
                {
                    continue;
                }
                int currentfd = conn->getFd();
                //判断fd是否有读事件触发
                //FD_ISSET宏检查currentfd是否在tempFds集合中，如果在，说明这个连接有数据可读
                if(FD_ISSET(currentfd, &tempFds))
                {
                    //5：处理就绪事件
                    //调用连接对象的handleEvent方法，处理这个连接上的事件（比如读取数据、处理请求等）
                    conn->readData();
                }
            }
    }
}

