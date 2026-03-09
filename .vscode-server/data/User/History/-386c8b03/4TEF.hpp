#include "Socket.hpp"
#include "Logger.hpp"
#include "InetAddr.hpp"
#include <memory>
#include <signal.h>

static uint16_t gport = 8080;
using namespace NS_SOCKET;

class TcpServer
{
public:
    TcpServer(uint16_t port = gport) : _port(port), _listensock(std::make_unique<TcpSocket>())
    {
        _listensock->BuildTcpSocketMethod(_port);
    }

    void Loop()
    {
        while (1)
        {
            InetAddr clientaddr;
            auto sockfd = _listensock->Accept(clientaddr);
            if (!sockfd)
            {
                continue;
            }
            LOG(LogLevel::DEBUG) << "get a new link,socket address:" << clientaddr.ToString() << "sockfd:" << sockfd->sockfd();
        }
    }
    ~TcpServer()
    {
    }

private:
    uint16_t _port;
    std::unique_ptr<Socket> _listensock;
};