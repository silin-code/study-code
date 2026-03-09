#ifndef __PROCESS_POOL_HPP__
#define __PROCESS_POOL_HPP__

#include <iostream>
#include <string>
#include <vector>
#include <cstdlib>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <functional>
#include "Task.cpp"
#include "Channel.hpp"

typedef std::function<void()> work_t;
// using work_t = std::function<void()>;

enum
{
    OK = 0,
    UsageError,
    PipeError,
    ForkError
};

class ProcessPool
{
public:
    ProcessPool(int n, work_t w) : processnum(n), work(w) {}

    int InitProcessPool()
    {
        //创建指定个数进程
        for(int i=0;i<processnum;i++)
        {
            //创建管道
            int pipefd[2]={0};
            int n= pipe(pipefd);
            if(n<0) return  PipeError;

            //创建进程
            pid_t id=fork();
            if(id<0) return ForkError;

            if(id==0)
            {
                std::cout<<getpid()<<",child close history fd:";
                for(auto&c : channels)
                {
                    std::cout<<c.wFd()<<" ";
                    c.Close();
                }
                std::cout<<"Over"<<std::endl;

                ::close(pipefd[1]);//关闭写端
                //child
                std::cout<<"debug"<<pipefd[0]<<std::endl;
                dup2(pipefd[0],0);
                work();
                ::exit(0);

            }
        }
    }

private:
    std::vector<Channel> channels;
    int processnum;
    work_t work;
}

#endif