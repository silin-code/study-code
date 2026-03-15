#include<iostream>
#include<cstring>
#include<sys/socket.h>
#include<netinet/in.h>
#include<unistd.h>
#include<cstdio>
#include<cstdlib>

#define PORT 8080

int main()
{
    int server_fd,new_socket;
    ssize_t valred;
    struct sockaddr_in address;
    int opt=1;
    int addrlen =sizeof(address);
    char buffer[1024];
    const char* response="hi from TCP server";

    
    return 0;
}