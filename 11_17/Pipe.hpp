#pragma once

#include <iostream>
#include <string>
#include <cstdlib>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <errno.h>
#include <cstring>
#include <fcntl.h>

const std::string gcommfile = "./fifo";
#define For_Read 0
#define ForwRite 1

class Fifo
{
public:
    Fifo(const std::string &commfile = gcommfile) : _commfile(commfile), _mode(0666), _fd(-1) {}
    // 1:creat pipe
    void Build()
    {
        if (IsExists())
            return;

        umask(0);
        int n = mkfifo(_commfile.c_str(), _mode);
        if (n < 0)
        {
            std::cerr << "mkfifo error" << strerror(errno) << "error" << errno << std::endl;
            exit(1);
        }
        std::cerr << "mkfifo sucess" << std::endl;
    }
    // 2:get pipe
    void Open(int mode)
    {
        if (mode == For_Read)
            _fd = open(_commfile.c_str(), O_RDONLY);
        else if (mode == ForwRite)
            _fd = open(_commfile.c_str(), O_WRONLY);
        else
        {
        }

        if (_fd < 0)
        {
            std::cerr << "open error:" << strerror(errno) << "error" << errno << std::endl;
            exit(2);
        }
        else
        {
            std::cout << "open file success" << std::endl;
        }
    }
    void Send(const std::string &msg)
    {
        ssize_t n = write(_fd, msg.c_str(), msg.size());
        (void)n;
    }
    int Recv(std::string *msgout)
    {
        char buf[128];
        ssize_t n = read(_fd, buf, sizeof(buf) - 1);
        if (n > 0)
        {
            buf[n] = 0;
            *msgout = buf;
            return n;
        }
        else if (n == 0)
        {
            return 0;
        }
        else
        {
            return -1;
        }
    }
    // 3: rm pipe
    void Delete()
    {
        int n = unlink(_commfile.c_str());
        (void)n;
    }
    ~Fifo() {}

private:
    bool IsExists()
    {
        int fd = open(_commfile.c_str(), O_RDONLY);
        return fd >= 0;
    }

private:
    std::string _commfile;
    mode_t _mode;
    int _fd;
};