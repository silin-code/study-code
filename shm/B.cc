// 接收端
#include <iostream>
#include <sys/ipc.h>
#include <string>
#include <cstring>
#include <stdlib.h>
#include <sys/shm.h>

#define SHM_SIZE 1024
#define KEY_PATH "."
#define KEY_ID 'a'

using namespace std;

int main()
{
    key_t key;
    int shmid;
    char *shm_addr;
    if ((key = ftok(KEY_PATH, KEY_ID)) == -1)
    {
        perror("ftok fail");
        exit(-1);
    }

    if((shmid=shmget(key,SHM_SIZE,0666))==-1)
    {
        perror("shmget fail");
        exit(-2);
    }

    if ((shm_addr = (char *)shmat(shmid, NULL, 0)) == (char *)-1)
    {
        perror("shmat fail");
        exit(-3);
    }

    cout<<"There is B process:"<<shm_addr<<endl;

    if(shmdt(shm_addr)==-1)
    {
        perror("shmdt fail");
        exit(-4);
    }

    if(shmctl(shmid,IPC_RMID,NULL)==-1){
        perror("shmctl fail");
        exit(-5);
    }

    cout<<"share end"<<endl;

    return 0;
}