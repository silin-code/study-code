// 发送端
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

    if ((shmid = shmget(key, SHM_SIZE, IPC_CREAT | 0666)) == -1)
    {
        perror("shget fail");
        exit(-2);
    }

    if ((shm_addr = (char *)shmat(shmid, NULL, 0)) == (char *)-1)
    {
        perror("shmat fail");
        exit(-3);
    }

    string s = "i am process A";
    strcpy(shm_addr, s.c_str());
    cout << "A has writed date" << s << endl;

    getchar();
    if(shmdt(shm_addr)==-1)
    {
        perror("shmdt fail");
        exit(-4);
    }
    return 0;
}