#include "../include/HttpServer.h"
// 新增Cookie和Session
#include "../include/CookieUtil.h"
#include "../include/SessionManager.h"
#include <iostream>
#include <fstream>
#include <sstream>
#include <cstring>
#include <unistd.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>

// 缓冲区大小(读取浏览器请求)
#define BUFFER_SIZE 4096

// 构造函数，初始化端口
HttpServer::HttpServer(int port) : port(port), server_fd(-1) {}

// 获取服务器公网IP
std::string HttpServer::getPublicIP()
{
    char buffer[128] = {0};
    // 调用Linux命令查看公网IP
    FILE *fp = popen("curl -s ifconfig.me", "r");
    if (fp)
    {
        fgets(buffer, sizeof(buffer), fp);
        pclose(fp);
    }

    if (strlen(buffer) == 0)
    {
        return "请手动查询公网IP";
    }
    return std::string(buffer);
}

// 1/初始化socket Tcp服务器(HTTP基于TCP)
bool HttpServer::initSocket()
{
    // 1:创建socket文件描述符
    server_fd = socket(AF_INET, SOCK_STREAM, 0);
    if (server_fd == -1)
    {
        std::cerr << "Socket 创建失败" << std::endl;
        return false;
    }

    // 2:设置端口复用(防止重启报错)
    int opt = 1;
    setsockopt(server_fd, SOL_SOCKET, SO_REUSEADDR | SO_REUSEPORT, &opt, sizeof(opt));

    // 3:绑定服务器IP和端口
    struct sockaddr_in address;
    address.sin_family = AF_INET;
    address.sin_addr.s_addr = INADDR_ANY;
    address.sin_port = htons(port);

    if (bind(server_fd, (struct sockaddr *)&address, sizeof(address)) < 0)
    {
        std::cerr << "端口绑定失败" << std::endl;
        return false;
    }

    // 4:监听端口(最大连接数5)
    if (listen(server_fd, 5) < 0)
    {
        std::cerr << "监听失败" << std::endl;
        return false;
    }
    // 自动打印公网IP
    std::string ip = getPublicIP();
    std::cout << "================================" << std::endl;
    std::cout << "服务器启动成功，监听窗口:" << port << std::endl;
    std::cout << "你的服务器公网IP:" << ip << std::endl;
    std::cout << "浏览器访问地址:http://:" << ip << ":" << port << std::endl;
    std::cout << "================================" << std::endl;
    return true;
}

// 前端处理函数
// 修改readFrontEndFile，加上错误日志，方便排查
std::string HttpServer::readFrontEndFile(const std::string &filename)
{
    std::string path = "web/" + filename;
    std::cout << "尝试读取文件: " << path << std::endl; // 加日志！
    std::ifstream file(path);
    if (!file.is_open())
    {
        std::cerr << "文件打开失败: " << path << std::endl; // 错误日志
        return "<h1>文件未找到!</h1>";
    }

    std::stringstream buffer;
    buffer << file.rdbuf();
    file.close();

    std::cout << "文件读取成功，大小: " << buffer.str().size() << " 字节" << std::endl;
    return buffer.str();
}

// // 处理客户端请求//版本1，不需要cookie和session
// void HttpServer::handleClient(int client_fd)
// {
//     char buffer[BUFFER_SIZE] = {0};
//     read(client_fd, buffer, sizeof(buffer));

//     // 打印请求
//     std::cout << "\n====浏览器HTTP请求====\n"
//               << buffer << "\n=====================\n";

//     // 1:读取独立前端页面
//     std::string html = readFrontEndFile();
//     // 2:标准HTTP响应协议
//     std::string response =
//         "HTTP/1.1 200 OK\r\n"
//         "Content-Type: text/html; charset=utf-8\r\n"
//         "Connection: close\r\n"
//         "\r\n" +
//         html;

//     // 发送给浏览器
//     write(client_fd, response.c_str(), response.size());
//     close(client_fd);
// }

// 升级版本2，使用Cookie和Session
void HttpServer::handleClient(int client_fd)
{
    char buffer[BUFFER_SIZE] = {0};
    read(client_fd, buffer, sizeof(buffer));
    std::string request(buffer);

    // 打印完整请求，方便查看cookie和请求头
    std::cout << "\n====浏览器HTTP请求====\n"
              << buffer << "\n=====================\n";

    //-------------------------First Step,解析cookie
    // 用CookieUtil从请求里面解析出所有cookie，比如SESSIONID=???
    auto cookies = CookieUtil::parseCookies(request);
    // 从Cookie里拿到SESSIONID,如果有
    std::string sessionId = cookies["SESSIONID"];
    // 获取全局唯一的Session管理器
    SessionManager &sessionMgr = SessionManager::getInstance();

    //------------------------Second Step，判断请求路径，处理不同业务
    std::string response;

    // 场景1，用户访问登录页面(GET /login.html)
    size_t loginPos = request.find("GET /login.html");
    std::cout << "查找GET /login.html的位置: " << loginPos << std::endl; // 新增日志
    if (loginPos != std::string::npos)
    {
        std::cout << "✅ 成功匹配到 /login.html 请求" << std::endl;
        std::string html = readFrontEndFile("login.html");
        std::cout << "📄 读取到的HTML内容长度: " << html.size() << std::endl;
        response = "HTTP/1.1 200 OK\r\n"
                   "Content-Type: text/html; charset=utf-8\r\n"
                   "Connection: close\r\n"
                   "\r\n" +
                   html;
    }
    // 场景2，用户提交登录请求(POST /login)
    else if (request.find("POST /login") != std::string::npos)
    {
        // 这里简化处理，直接用固定用户名"test_user"，你可以改成从POST请求里读用户名密码
        std::string username = "test_user";

        // 1. 调用SessionManager创建新的Session，生成SessionID
        std::string newSessionId = sessionMgr.createSession(username);
        // 2. 调用CookieUtil构建Set-Cookie响应头，把SessionID发给浏览器
        std::string setCookieHeader = CookieUtil::buildSetCookie("SESSIONID", newSessionId);

        // 3. 登录成功，用302重定向跳转到首页（浏览器收到302会自动跳转到Location指定的地址）
        response = "HTTP/1.1 302 Found\r\n" + setCookieHeader + "Location: /\r\n" // 跳转到首页
                   + "\r\n";
    }

    // 场景3：用户点击登出（GET /logout）
    else if (request.find("GET /logout") != std::string::npos)
    {
        // 1. 调用SessionManager销毁Session
        sessionMgr.destroySession(sessionId);
        // 2. 调用CookieUtil构建清除Cookie的响应头，让浏览器删掉SESSIONID
        std::string clearCookieHeader = CookieUtil::buildClearCookie("SESSIONID");

        // 3. 重定向到登录页
        response = "HTTP/1.1 302 Found\r\n" + clearCookieHeader + "Location: /login.html\r\n" + "\r\n";
    }

    // 场景4：用户访问首页（GET /）
    else if (request.find("GET / HTTP") != std::string::npos || request.find("GET /?") != std::string::npos)
    {
        // 校验Session是否有效（用户是否登录）
        if (sessionMgr.isSessionValid(sessionId))
        {
            // 已登录：获取用户名，显示欢迎信息
            std::string username = sessionMgr.getUsername(sessionId);
            // 读取index.html，把用户名替换进去（或者直接写死，这里简化处理）
            std::string html = "<h1>Hello World! 🎉</h1>"
                               "<p>欢迎你，" +
                               username + "！你已经成功登录啦~</p>"
                                          "<a href='/logout'>点击这里登出</a>";
            response = "HTTP/1.1 200 OK\r\n"
                       "Content-Type: text/html; charset=utf-8\r\n"
                       "Connection: close\r\n"
                       "\r\n" +
                       html;
        }
        else
        {
            // 未登录：重定向到登录页
            response = "HTTP/1.1 302 Found\r\n"
                       "Location: /login.html\r\n"
                       "\r\n";
        }
    }

    // 场景5：其他请求（比如访问不存在的页面），返回404
    else
    {
        response = "HTTP/1.1 404 Not Found\r\n"
                   "Content-Type: text/html; charset=utf-8\r\n"
                   "Connection: close\r\n"
                   "\r\n<h1>404 页面不存在</h1>";
    }

    // 把响应发给浏览器
    write(client_fd, response.c_str(), response.size());
    close(client_fd);
}

// 启动服务器
void HttpServer::start()
{
    if (!initSocket())
        return;
    struct sockaddr_in client_addr;
    socklen_t addr_len = sizeof(client_addr);

    while (true)
    {
        int client_fd = accept(server_fd, (struct sockaddr *)&client_addr, &addr_len);
        std::cout << "新链接:" << inet_ntoa(client_addr.sin_addr) << std::endl;
        handleClient(client_fd);
    }
}

// 关闭服务器
void HttpServer::stop()
{
    if (server_fd != -1)
        close(server_fd);
}