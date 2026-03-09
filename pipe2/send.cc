#include <iostream>
#include <string>
#include <unistd.h>
#include <sys/stat.h>
#include <fcntl.h>
using namespace std;

int main()
{
    umask(0);
    if (mkfifo("pipe", 0644) == -1)
    {
        perror("pipe exist");
    }
    int send = open("pipe", O_WRONLY);
    if (send == -1)
    {
        perror("open fail");
        unlink("pipe");
        exit(1);
    }
    string s = "i am process send";
    ssize_t write_len = write(send, s.c_str(), s.size());
    if (write_len == -1)
    {
        perror("write fail");
    }
    else
    {
        cout << "write success" << endl;
    }
    close(send);
    unlink("pipe");

    return 0;
}