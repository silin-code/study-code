#include "Logger.hpp"
#include "ThreadPool.hpp"

#include <iostream>
#include <memory>
#include <functional>
#include <ctime>
#include <cstdlib>

using namespace NS_LOG_MODULE;
using namespace NS_THREAD_POOL_MODULE;

// using task_t = std::function<void()>;

class Task
{
public:
    Task() {}
    Task(int x, int y) : _x(x), _y(y)
    {
    }
    void operator()()
    {
        _result = _x + _y;
    }
    std::string Result()
    {
        return std::to_string(_x) + "+" + std::to_string(_y) + "=" + std::to_string(_result);
    }
    ~Task() {}

private:
    int _x;
    int _y;
    int _result;
};

int main()
{
    // 创建对象的时机
    // 单例模式，就是只允许在加载或者运行期间，整体最多创建一个该类对象
    // 1. 加载到内存的时候，创建对象， int gval = 100;
    // 2. 进程在运行期间，创建对象, int *val = (int *)malloc(sizeof(int)); -- 最佳实践
    ENABLE_CONSOLE_LOG_STRATEGY();
    srand((long)time(nullptr) ^ getpid());
    std::unique_ptr<ThreadPool<Task>> tp = std::make_unique<ThreadPool<Task>>();
    tp->Start();

    int cnt = 10;
    while (cnt--)
    {
        int x = rand() % 10 + 1;
        usleep(137);
        int y = rand() % 20;
        Task t(x, y);
        tp->Enqueue(t);
        sleep(1);

        // tp->Enqueue([](){
        //         LOG(LogLevel::DEBUG) << "我是一个任务, 正在被处理...";
        //     }
        // );
        // sleep(1);
    }

    tp->Stop();
    tp->Wait();

    // sleep(5);

    // ThreadPool tp;
    // tp->Start();

    // while(true)
    // {
    //     // 获取任务
    //     // tp->Enqueue(t);
    // }

    // tp->Stop();
    return 0;
}