#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/wait.h>

int main()
{
    pid_t pid=fork();
    if(pid==0)
    {//child
        printf("I am child process\n");
        sleep(5);
        exit(25);
    }
    else if(pid>0)
    {//parent
        int status;
        pid_t wait_pid = waitpid(pid,&status,0);
        if(wait_pid==pid){
            //analyze exit status
             if(WIFEXITED(status)){
                printf("Child exit normally,exit ID:%d\n",WEXITSTATUS(status));
             }
             if(WIFSIGNALED(status)){
                printf("Child is stopped by signal,signal ID is:%d\n",WTERMSIG(status));
             }
             if(WCOREDUMP(status)){
                printf("Child creat come dump\n");
             }
             else
             {
                printf("Child dont't creat core dump\n");
              }
         }
    }
    else
    {
        perror("fork perror");
        exit(1);
    }
    return 0;
}

