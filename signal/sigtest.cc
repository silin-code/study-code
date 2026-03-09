#include <iostream>
#include <unistd.h>
#include <signal.h>

using namespace std;

void handler(int signumber)
{
    cout << "I am:" << getpid() << ".I get a sig:" << signumber << endl;
}

int main()
{
    cout << "I am a process,my pid:" << getpid() << endl;
    signal(SIGINT, handler);
    while (1)
    {
        cout << "I am a process:" << getpid() << ",i am waiting a sig" << endl;
        sleep(1);
    }
    return 0;
}
