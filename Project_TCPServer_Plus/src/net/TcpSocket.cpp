#include "TcpSocket.hpp"

//构造函数:初始化fd=-1;
TcpSocket::TcpSocket() : m_listenFd(-1) {
    LOG_DEBUG("TcpSocket initialized with fd = %d", m_listenFd);
}

//析构函数:关闭套接字
TcpSocket::~TcpSocket()
{
    Utils::closefd(m_listenFd);
}

//1创建套接字socket(AF_INET, SOCK_STREAM, 0);
// 系统调用：socket()
// 作用：创建一个 TCP 套接字
// 参数：
//    AF_INET     → IPv4 协议
//    SOCK_STREAM → TCP 面向连接
//    0           → 自动选择协议
// 返回值：成功返回文件描述符 fd，失败返回 -1
bool TcpSocket::create()
{
    m_listenFd = socket(AF_INET, SOCK_STREAM, 0);
    if (Utils::checkError(m_listenFd, "Socket 创建")) {
        LOG_INFO("TCP Socket created listening with fd = %d", m_listenFd);
        return true;
    }
    return false;
}

// 2. 设置端口复用 
// 系统调用：setsockopt()
// 作用：允许端口快速复用，解决服务器重启失败问题
// 参数：
//    fd         → 套接字文件描述符
//    SOL_SOCKET  → 套接字级别选项
//    SO_REUSEADDR → 允许地址复用
//    &opt       → 选项值（非零启用）
//    sizeof(opt) → 选项值大小
// 返回值：成功返回 0，失败返回 -1
void TcpSocket::serveReuseAddr()
{
    int opt=1;
    int ret = setsockopt(m_listenFd, 
        SOL_SOCKET, 
        SO_REUSEADDR, 
        &opt, 
        sizeof(opt));
    
    if(Utils::checkError(ret, "设置端口复用"))
    {
        LOG_INFO("端口复用设置成功,fd = %d", m_listenFd);
    }   
}

//3绑定IP和端口
//系统调用：bind()
//作用：将套接字绑定到指定的 IP 地址和端口号
//参数：
//    fd       → 套接字文件描述符
//    port     → 端口号（主机字节序）
//返回值：成功返回 0，失败返回 -1
bool TcpSocket::bind(uint16_t port)
{
    struct sockaddr_in serverAddr;//服务器地址结构体,作用是存储服务器的IP地址和端口信息

    //清空内存
    memset(&serverAddr, 0, sizeof(serverAddr));

    serverAddr.sin_family = AF_INET; // IPv4
    serverAddr.sin_addr.s_addr = INADDR_ANY; // 绑定到所有可用接口
    serverAddr.sin_port = Utils::htons(port); // 端口号转换为网络字节序

    int ret = ::bind(m_listenFd, (struct sockaddr*)&serverAddr, sizeof(serverAddr));
    if(Utils::checkError(ret, "绑定套接字"))
    {
        LOG_INFO("套接字绑定成功,fd = %d, port = %d", m_listenFd, port);
        return true;
    }
    return false;
}

//4监听套接字
//系统调用：listen()
//作用：将套接字设置为监听状态，准备接受客户端连接
//参数：
//    fd       → 套接字文件描述符
//    backlog  → 连接请求队列的最大长度
//返回值：成功返回 0，失败返回 -1
bool TcpSocket::listen(int backlog)
{
    //::前缀表示调用全局命名空间中的listen函数，避免与类成员函数冲突
    int ret=::listen(m_listenFd, backlog);//置为LISTEN状态，等待客户端SYN,握手
    if(Utils::checkError(ret, "监听窗口"))
    {
        LOG_INFO("服务器监听成功,fd = %d,队列长度:backlog = %d", m_listenFd, backlog);
    }
    return ret == 0;//ret==0表示成功，返回true；否则返回false
}


//5接受客户端连接
//系统调用：accept()
//作用：接受一个客户端连接请求，返回一个新的套接字文件描述符用于与客户端通信
//参数：
//    fd       → 监听套接字文件描述符
//    clientAddr → 存储客户端地址信息的结构体指针
//    addrLen   → 客户端地址结构体的大小
//返回值：成功返回新的套接字文件描述符，失败返回 -1
int TcpSocket::accept(sockaddr_in &clientAddr)//参数：clientAddr是一个引用参数，用于存储客户端的地址信息
{
    socklen_t len= sizeof(clientAddr);
    int clientfd=::accept(m_listenFd, (struct sockaddr*)&clientAddr, &len);//已经完成三次握手
    //内核完成三次握手后返回一个新的套接字描述符用于与客户端通信,同时将客户端地址信息存储在clientAddr中
    setNonBlocking(clientfd);
    //只有clientfd不为-1时才表示成功接受连接，才获取客户端IP和端口信息并打印日志
    if(Utils::checkError(clientfd, "接受客户端连接"))
    {     
        //获取客户端IP和端口信息
        std::string clientIP = Utils::getIPString(clientAddr);
        LOG_INFO("接受客户端连接:%s,fd=%d", clientIP.c_str(), clientfd);

        return clientfd;//返回新的套接字描述符，用于与客户端通信
    }
    return -1;
}

//工具函数
//1设置套接字为非阻塞模式
//系统调用：fcntl()
//作用：将套接字设置为非阻塞模式，允许异步处理 IO 操作
//参数：
//    fd       → 套接字文件描述符
//返回值：无
void TcpSocket::setNonBlocking(int fd)
{
    int flags = fcntl(fd, F_GETFL, 0);
    Utils::checkError(flags, "获取文件状态标志");
    
    flags |= O_NONBLOCK; // 设置非阻塞标志
    int ret = fcntl(fd, F_SETFL, flags);
    Utils::checkError(ret, "设置非阻塞模式");

    LOG_DEBUG("套接字fd=%d设置为非阻塞模式", fd);
}

//2获取监听sokcet的fd
int TcpSocket::getListenFd() const
{
    return m_listenFd;
}