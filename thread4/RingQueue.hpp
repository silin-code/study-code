#pragma once

#include <iostream>
#include <string>
#include <vector>
#include <pthread.h>
#include <unistd.h>
#include "Sem.hpp"
#include "Mutex.hpp"
const int defaultcap = 5;

template <class T>
class RingQueue
{
public:
    RingQueue(int cap = defaultcap)
        : _cap(cap),
          _rq(cap),
          _consumer_step(0),
          _productor_step(0),
          _blank_sem(cap),
          _data_sem(0)
    {
    }

    void Enqueue(T &in)
    {
        _pmutex.Lock();
        _blank_sem.P();
        _rq[_productor_step++] = in;
        _productor_step %= _cap;

        _data_sem.V();
        _pmutex.Unlock();
    }
    void Pop(T *out)
    {
        _cmutex.Lock();
        _data_sem.P();
        *out = _rq[_consumer_step++];
        _consumer_step %= _cap;

        _blank_sem.V();
        _cmutex.Unlock();
    }

    ~RingQueue()
    {
    }

private:
    int _cap;
    std::vector<T> _rq;

    int _consumer_step;
    int _productor_step;

    Sem _blank_sem;
    Sem _data_sem;

    Mutex _cmutex;
    Mutex _pmutex;
};