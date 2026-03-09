#pragma once

#include <iostream>
#include <sys/shm.h>
#include <sys/ipc.h>

const int gsize = 4096;
#define PYTHNAME "/tmp"
#define PROJ_ID 0x66

class Shm
{
public:
    Shm() : _shmid(-1), _size(gsize), _start_addr(nullptr) {}
    void Create()
    {
        key_t k = Getkey();
        if (k < 0)
        {
            std::cerr << "Getkey error";
            exit(1);
        }
        _shmid = shmget(k, _size, IPC_CREAT | IPC_EXCL | 0666);
        if (_shmid < 0)
        {
            perror("shmget");
            exit(2);
        }
    }
    void Delete()
    {
        int n=shmdt(_start_addr);
        (void)n;   
    }

    void Attach()
    {
        _start_addr = shmat(_shmid, nullptr, 0);
        {
            if ((long long int)_start_addr == -1)
            {
                exit(3);
            }
        }
    }

    void Attr()
    {
        struct shmid_ds ds;
        int n = shmctl(_shmid,IPC_STAT,&ds);
        if(n<0)
        {
            perror("shmctl");
            exit(4);
        }
    }

    void get()
    {
        key_t k = Getkey();
        if (k < 0)
        {
            std::cerr << "Getkey error";
            exit(1);
        }
        _shmid = shmget(k, _size, IPC_CREAT | IPC_EXCL | 0666);
    }

private:
    key_t Getkey()
    {
        return ftok(PYTHNAME, PROJ_ID);
    }

private:
    int _shmid;
    int _size;
    void *_start_addr;
};