#include <iostream>
#include <signal.h>
#include <unistd.h>
#include <stdlib.h>
using namespace std;

void sigcb(int signo)
{
    cout << "Get signal number:" << signo << endl;
}

int main()
{
    signal(SIGINT, sigcb);
    signal(SIGRTMIN + 6, sigcb);

    sigset_t block_set;
    sigemptyset(&block_set);
    sigaddset(&block_set, SIGINT);
    sigaddset(&block_set, SIGRTMIN + 6);

    if (sigprocmask(SIG_BLOCK, &block_set, nullptr) == -1)
    {
        perror("sigcpromask fail");
        exit(EXIT_FAILURE);
    }

    cout << "Process ID:" << getpid() << endl;
    printf("=== 信号阻塞中 ===\n");
    printf("当前进程ID: %d\n", getpid());
    printf("请在15秒内，另开终端执行以下命令发送信号（各5次）：\n");
    printf("kill -2 %d  # 发送2号信号（非可靠）\n", getpid());
    printf("kill -40 %d # 发送40号信号（可靠）\n", getpid());
    sleep(15);
    if (sigprocmask(SIG_UNBLOCK, &block_set, NULL) == -1)
    {
        perror("sigprocmask 解除阻塞失败");
        exit(EXIT_FAILURE);
    }
    printf("\n=== 已解除阻塞，开始处理待处理信号 ===\n");
    sleep(5);
    return 0;
}