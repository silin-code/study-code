#include <iostream>
#include <signal.h>

void Usage(const std::string &cmd)
{
    std::cout << "Usage:" << cmd << "signumber who" << std::endl;
}

void handler(int sig)
{
    std::cout << "进程捕捉信号" << sig << "pid:" << getpid() << std::endl;
}

int main()
{
    while (true)
    {
        std::cout << "进程正在运行" << getpid() << std::endl;
        sleep(2);
        raise(9);
    }
    return 0;
}