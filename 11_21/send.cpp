#pragma once
#include <pthread.h>
#include <arpa/inet.h>
#include <netinet/in.h>
#include <sys/socket.h>
#include <unistd.h>
#include <iostream>
#include <cstring>
#include <sys/wait.h>
#include <sys/socket.h>
#include <iostream>
#include <stdio.h>
#include <unistd.h>
#include <cstdarg>
#include <cstring>
#include <time.h>
#include <random>       
#include <thread>      
#include <chrono>      
#include <semaphore.h>
#include <arpa/inet.h>
#include <netinet/in.h>


void geterrno()
{
    std::cout<<"请输入正确参数!"<<std::endl;
}

int main(int argc,char* argv[])
{
    if(argc!=3)
    {
        geterrno();
        exit(0);
    }

    //创建套接字
    int token = socket(AF_INET, SOCK_STREAM, 0);
   
    //客户端发送请求
    struct sockaddr_in addr;
    memset(&addr, 0, sizeof(addr));
    addr.sin_family=AF_INET;
    inet_pton(AF_INET,argv[1] , &addr.sin_addr.s_addr);
    addr.sin_port=htons(atoi(argv[2]));

    socklen_t ip_addr=sizeof(addr);
    int can = connect(token,(const sockaddr*)&addr,ip_addr);
   
    char buffer[1024]={0};
    //bool connected = true;
    while(1)
    {
        memset(buffer, 0, sizeof(buffer));
        std::cout<<"请输入发送内容........."<<std::endl;
         getchar();
         scanf("%s", buffer);
         write(token,buffer,sizeof(buffer)-1);
        
        //接收服务器数据
        ssize_t recv_len = recv(token, buffer, sizeof(buffer)-1, 0);
        if(recv_len>0)
        {
            buffer[recv_len] = '\0';
            std::cout<<buffer<<std::endl;
            std::cout<<"此次任务执行完毕，已和服务器断开连接"<<std::endl;
            break;
        }
        // else if(recv_len==0)
        // {
        //     std::cout<<"此次任务执行完毕，已和服务器断开连接"<<std::endl;
        //     connected=false;
        //     break;
        // }
    }
    close(token);

    return 0;
}