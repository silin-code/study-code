#pragma once

#include <vector>
#include <queue>
#include "Thread.hpp"
#include "Logger.hpp"
#include "Cond.hpp"

namespace NS_THREAD_POOL_MODULE
{
    using namespace NS_LOG_MODULE;
    using namespace NS_THREAD_MODULE;

    const int defaultnum = 5;

    template <class T>
    class ThreadPool
    {
    public:
        void HandlerTask()
        {
            char name[128];
            pthread_getname_np(pthread_self(), name, sizeof(name));
            while (1)
            {
                T task;
                {
                    LockGuard lkg(_mutex);
                    // 检测任务。不休眠：1. 队列不为空 2. 线程池退出 -> 队列为空 && 线程池不退出
                    while (_tasks.empty() && _isrunning)
                    {
                        _slavers_sleep_count++;
                        _cond.Wait(_mutex);
                        _slavers_sleep_count--;
                    }
                    // 线程池退出了-> while 就要break -> 不能
                    // 1. 线程池退出 && _tasks empty
                    if (!_isrunning && _tasks.empty())
                    {
                        break;
                    }

                    task = _tasks.front();
                    _tasks.pop();
                }

                LOG(LogLevel::INFO) << name << "处理ing";
                task();
                LOG(LogLevel::DEBUG) << task.Result();
            }
        }

    public:
        ThreadPool(int slaver_num = defaultnum)
            : _isrunning(false),
              _slavers_sleep_count(0),
              _slaver_num(slaver_num)
        {
            for (int idx = 0; idx < _slaver_num; idx++)
            {
                _slavers.emplace_back([this]()
                                      { this->HandlerTask(); });
            }
        }

        void Start()
        {
            if (_isrunning)
            {
                LOG(LogLevel::WARING) << "Thread Pool Is Already Running";
                return;
            }
            _isrunning = true;
            for (auto &slave : _slavers)
            {
                slave.Start();
            }
        }

        void Stop()
        {
            _mutex.Lock();
            _isrunning = false;
            if (_slavers_sleep_count > 0)
                _cond.Broadcast();
            _mutex.Unlock();
        }

        void Wait()
        {
            for (auto &slave : _slavers)
            {
                slave.Join();
            }
        }

        void Enqueue(T in)
        {
            _mutex.Lock();
            _tasks.push(in);
            if (_slavers_sleep_count > 0)
            {
                _cond.Signal();
            }
            _mutex.Unlock();
        }
        ~ThreadPool()
        {
        }

    private:
        bool _isrunning;
        int _slaver_num;
        std::vector<Thread> _slavers;
        std::queue<T> _tasks;
        Mutex _mutex;
        Cond _cond;
        int _slavers_sleep_count;
    };
}
