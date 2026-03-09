#include <stdio.h>
#include <unistd.h>
#include <sys/types.h>
#include <string.h>
#include <sys/wait.h>
#include <stdlib.h>
int main()
{
    int fd[2] = {0};
    int i = pipe(fd);
    if (i < 0)
    {
        perror("pipe fail");
        exit(1);
    }
    pid_t id = fork();
    if (id < 0)
    {
        perror("fork fail");
        exit(1);
    }
    else if (id == 0)
    { // child read
        close(fd[1]);
        char buff[1024]={0};
        read(fd[0], buff, 1023);
        printf("%s", buff);
        close(fd[0]);
        return 1;
    }
    else
    {
        // father write
        close(fd[0]);
        const char *buff = "i am father";
        write(fd[1], buff, strlen(buff));
        close(fd[1]);
        wait(NULL);
    }

    return 0;
}
