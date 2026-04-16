#ifndef HTTPSERVER_H
#define HTTPSERVER_H

#include<string>

//HTTP服务器类
class HttpServer{
private:
    int port;//监听端口
    int server_fd;//服务器socket文件描述符

    //初始化Socket(创建，绑定，监听)
    bool initSocket();
    //自动获取服务器公网IP
    std::string getPublicIP();
    //前后端分离，读取前端html文件，构造响应
    std::string readFrontEndFile();
    //解析并处理客户端请求
    void handleClient(int client_fd);

public:
    //构造函数
    HttpServer(int port);
    //启动服务器
    void start();
    //关闭服务器
    void stop();
};

#endif //HTTPSERVER_H