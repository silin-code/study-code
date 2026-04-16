#include "../include/HttpServer.h"
#define PORT 8080

int main()
{
    HttpServer server(PORT);
    server.start();
    server.stop();
    return 0;
}