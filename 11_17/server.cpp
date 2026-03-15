#include "Pipe.hpp"

int main()
{
    Fifo pipefile;
    pipefile.Build();
    pipefile.Open(For_Read);

    std::string msg;
    while (true)
    {
        int n = pipefile.Recv(&msg);
        if (n > 0)
            std::cout << "Clinet Say:" << msg << std::endl;
        else
            break;
    }
    pipefile.Delete();
    return 0;
}