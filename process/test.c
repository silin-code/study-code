#include<stdio.h>
#include<unistd.h>
#include<sys/wait.h>

int main()
{
    pid_t pid = fork();
    if(pid==0)
    {
        //child
        execlp("ls","ls","-l","-a",NULL);
        perror("execlp fail");
    }
    else if(pid>0)
    {
        //parent
        wait(NULL);
        printf("child process (ls - la) has finished");
    }
    else
    {
        perror("fork fail");
        return 1;
    }
    return 0;
}
