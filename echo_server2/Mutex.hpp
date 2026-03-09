#ifndef __Mutex_HPP
#define __Mutex_HPP

#include <pthread.h>

class Mutex
{
public:
    Mutex()
    {
        pthread_mutex_init(&_mutex, nullptr);
    }

    void Lock()
    {
        pthread_mutex_lock(&_mutex);
    }

    void Unlock()
    {
        pthread_mutex_unlock(&_mutex);
    }

    pthread_mutex_t *Ptr()
    {
        return &_mutex;
    }
    ~Mutex()
    {
        pthread_mutex_destroy(&_mutex);
    }

private:
    pthread_mutex_t _mutex;
};

class LockGuard
{
public:
    LockGuard(Mutex &lock) : _lockref(lock)
    {
        _lockref.Lock();
    }

    ~LockGuard()
    {
        _lockref.Unlock();
    }

private:
    Mutex &_lockref;
};

#endif