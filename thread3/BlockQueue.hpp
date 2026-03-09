#ifndef __BLOCK_QUEUE_H
#define __BLOCK_QUEUE_H

#include <iostream>
#include <queue>
#include <pthread.h>
#include <unistd.h>
#include <cstdlib>
#include <ctime>
#include<string>
#include<vector>
const int defaultcap = 5;

template <class T>
class BlockQueue
{
public:
    BlockQueue(int cap = defaultcap) : _cap(cap)
    {
        pthread_mutex_init(&_mutex, nullptr);
        pthread_cond_init(&_consumer_cond, nullptr);
        pthread_cond_init(&_productor_cond, nullptr);
        // _blockqueue_high_water = _cap * 2 / 3;
        // _blockqueue_low_water = _cap * 1 / 3;

        sleep_consumer_num = 0;
        sleep_productor_num = 0;
    }

    void Enqueue(T &in)
    {
        pthread_mutex_lock(&_mutex);

        if (_dp.size() == _cap)
        {
            sleep_productor_num++;
            pthread_cond_wait(&_productor_cond, &_mutex);
            sleep_productor_num--;
        }

        _dp.push(in);
        // if (_dp.size() > _blockqueue_high_water)
        if (sleep_consumer_num > 0)
            pthread_cond_signal(&_consumer_cond);
        pthread_mutex_unlock(&_mutex);
    }

    void pop(T *out) // consumer
    {
        pthread_mutex_lock(&_mutex);
        if (_dp.empty())
        {
            sleep_consumer_num++;
            pthread_cond_wait(&_consumer_cond, &_mutex);
            sleep_consumer_num--;
        }
        *out = _dp.front();
        _dp.pop();
        // if(_dp.size()<_blockqueue_low_water)
        if (sleep_productor_num > 0)
            pthread_cond_signal(&_productor_cond);
        pthread_mutex_unlock(&_mutex);
    }

    ~BlockQueue()
    {
        pthread_mutex_destroy(&_mutex);
        pthread_cond_destroy(&_consumer_cond);
        pthread_cond_destroy(&_productor_cond);
    }

private:
    std::queue<T> _dp;
    int _cap;

    pthread_mutex_t _mutex;
    pthread_cond_t _consumer_cond;
    pthread_cond_t _productor_cond;

    // int _blockqueue_low_water;
    // int _blockqueue_high_water;

    int sleep_productor_num;
    int sleep_consumer_num;
};

#endif