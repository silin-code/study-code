#include <iostream>
#include <string>
#include <unistd.h>
#include <sys/stat.h>
#include <fcntl.h>
using namespace std;

int main()
{
    int recv = open("pipe", O_RDONLY);
    if (recv == -1)
    {
        perror("open fail");
        exit(1);
    }
    cout << "serve open sucess" << endl;
    char buff[1024] = {0};
    ssize_t read_len = read(recv, buff, sizeof(buff) - 1);
    if(read_len==-1)
    {
        perror("read fail");
    }
    else if(read_len==0)
    {
        cout<<"pipe write close"<<endl;
    }
    else{
        cout<<"read_len:"<<buff<<endl;
    }
    close(recv);
    unlink("pipe");
    return 0;
}