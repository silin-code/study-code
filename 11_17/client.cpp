#include "Pipe.hpp"

int main()
{
    Fifo fileclient;
    fileclient.Open(ForwRite);

    while (true)
    {
        std::cout << "请输入" << std::endl;
        std::string msg;
        std::getline(std::cin, msg);
        fileclient.Send(msg);
    }

    return 0;
}