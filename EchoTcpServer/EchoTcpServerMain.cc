#include "EchoTcpServer.hpp"
#include <memory>

int main()
{
    std::unique_ptr<TcpServer> tsvr = std::make_unique<TcpServer>();
    tsvr->Start();
    return 0;
}
