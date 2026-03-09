#include "Thread.hpp"

using namespace NS_THREAD_MODULE;

void Task()
{
    while (1)
    {
        std::cout << "This is task" << std::endl;
        sleep(1);
    }
}

int main()
{
    Thread t1(Task);
    Thread t2(Task);
    t1.Start();
    t2.Start();
    
    return 0;
}