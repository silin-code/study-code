#include<stdio.h>
#include<unistd.h>
#include<pthread.h>

void threadRun()
{
    while(1)
    {
        printf("newthread is runnong,pid:%d\n",getpid());
        sleep(1);
    }
}

int main()
{
    pthread_t tid;
    pthread_create(&tid,NULL,threadRun,NULL);
    while(1)
    {
        printf("main thread is running,pid:%d\n",getpid());
    }
    return 0; 
}