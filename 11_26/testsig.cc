#include <iostream>
#include <unistd.h>
#include <signal.h>

void PrintPending(sigset_t &pending)
{
    for (int signo = 1; signo <= 32; signo++)
    {
        if (sigismember(&pending, signo))
        {
            std::cout << "1";
        }
        else
        {
            std::cout << "0";
        }
        std::cout << std::endl;
    }
}

int main()
{
    sigset_t block_set, old_set;
    sigemptyset(&block_set);
    sigemptyset(&old_set);
    sigaddset(&block_set, 2);

    int n = sigprocmask(SIG_SETMASK, &block_set, &old_set);
    (void)n;

    int cnt = 1;

    std::cout << "pid:" << getpid() << std::endl;
    while (true)
    {
        sigset_t pending;
        sigemptyset(&pending);
        
    }

    return 0;
}