#include<cstdio>
#include<unistd.h>

int main()
{
    pid_t pid=fork();
    if(pid==0)
    {
        while(1)
        {
            printf("A");
            usleep(1240);
            
            printf("A");
            usleep(1230);

            printf("\n");
        }
    }
    else{
        while(1)
        {
            
        }
    }
}