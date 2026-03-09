#pragma once
#include <iostream>
#include <pthread.h>
#include <functional>
#include <string>
#include <unistd.h>

namespace NS_THREAD_MODULE
{
    static int gnumber = 1;
    using callback_t = std::function<void()>;

    enum class STATUS
    {
        THREAD_NEW,
        THREAD_RUNNING,
        THREAD_STOP
    };

    class Thread
    {
    private:
        void ToRunning()
        {
            _status = STATUS::THREAD_RUNNING;
        }

        void ToStop()
        {
            _status = STATUS::THREAD_STOP;
        }
        static void *ThreadRoutin(void *args) // 不存在this指针
        {
            Thread *self = static_cast<Thread *>(args);
            self->ToRunning();
            pthread_setname_np(self->_tid, self->_name.c_str());
            self->_cb();

            self->ToStop();
            return nullptr;
        }

    public:
        Thread(callback_t cb) : _tid(-1), _status(STATUS::THREAD_NEW), _joinable(true)
        {
            _name = "Thread" + std::to_string(gnumber++);
        }

        bool Start()
        {
            int n = pthread_create(&_tid, nullptr, ThreadRoutin, nullptr);
            if (n != 0)
                return false;
            return true;
        }

        void Join()
        {
            if (_joinable)
            {
                int n = pthread_join(_tid, &_result);
                if (n != 0)
                {
                    std::cerr << "join fail" << n << std::endl;
                }
                (void)_result;
            }
            else
            {
                std::cerr << "error,thread join statud:" << _joinable << std::endl;
            }
        }

        void Stop()
        {
            if (_status == STATUS::THREAD_RUNNING)
            {
                pthread_cancel(_tid);
                _status = STATUS::THREAD_STOP;
            }
        }

        void Detach()
        {
            if (_status == STATUS::THREAD_RUNNING)
            {
                pthread_detach(_tid);
                _joinable = false;
            }
            else
            {
                std::cerr << "Detach" << _name << "fail" << std::endl;
            }
        }

        ~Thread()
        {
        }

    private:
        std::string _name;
        pthread_t _tid;
        STATUS _status;
        bool _joinable;

        callback_t _cb;

        void *_result;
    };

}
