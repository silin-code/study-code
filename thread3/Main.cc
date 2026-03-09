#include "BlockQueue.hpp"
#include <iostream>
#include <string>
#include <ctime>
#include <pthread.h>
#include <unistd.h>
#include <mutex>
#include <random>

int num = 0;
pthread_mutex_t lock = PTHREAD_MUTEX_INITIALIZER;

// 线程安全的编号生成函数
int Getnumber()
{
    pthread_mutex_lock(&lock);
    int number = num++;
    pthread_mutex_unlock(&lock);
    return number;
}

// 线程安全的随机数生成器（替代rand()）
int ThreadSafeRand()
{
    static thread_local std::mt19937 gen(std::random_device{}());
    std::uniform_int_distribution<int> dis(0, 9);
    return dis(gen);
}

// 消费者线程函数
void *Consumer(void *args)
{
    int number = Getnumber();
    std::string name = "Consumer-" + std::to_string(number);
    // 限制线程名长度（Linux下最大15字符）
    if (name.size() > 15)
        name = name.substr(0, 15);
    pthread_setname_np(pthread_self(), name.c_str());

    BlockQueue<int> *dp = static_cast<BlockQueue<int> *>(args);
    while (1)
    {
        sleep(1); // 消费速度慢于生产，体现缓冲区作用
        int data;
        dp->pop(&data);
        std::cout << name << "消费:" << data << std::endl;
    }
    return nullptr;
}

// 生产者线程函数
void *Productor(void *args)
{
    int number = Getnumber();
    std::string name = "Productor-" + std::to_string(number);
    if (name.size() > 15)
        name = name.substr(0, 15);
    pthread_setname_np(pthread_self(), name.c_str());

    BlockQueue<int> *dp = static_cast<BlockQueue<int> *>(args);
    while (1)
    {
        int data = ThreadSafeRand(); // 线程安全随机数
        dp->Enqueue(data);
        std::cout << name << "生产:" << data << std::endl;
        usleep(500000); // 生产速度快于消费
    }
    return nullptr;
}

int main()
{
    // 创建阻塞队列（缓冲区容量5）
    BlockQueue<int> *bp = new BlockQueue<int>(5);

    // 定义线程ID数组（明确指定下标，提升可读性）
    pthread_t c[2] = {0}; // 2个消费者
    pthread_t p[3] = {0}; // 3个生产者

    // 正确创建线程：传入线程ID的地址
    pthread_create(&c[0], nullptr, Consumer, bp);
    pthread_create(&c[1], nullptr, Consumer, bp);

    pthread_create(&p[0], nullptr, Productor, bp);
    pthread_create(&p[1], nullptr, Productor, bp);
    pthread_create(&p[2], nullptr, Productor, bp);

    // 等待线程退出（实际死循环不会执行到这里，可加信号处理退出）
    pthread_join(c[0], nullptr);
    pthread_join(c[1], nullptr);
    pthread_join(p[0], nullptr);
    pthread_join(p[1], nullptr);
    pthread_join(p[2], nullptr);

    // 释放内存（避免泄漏）
    delete bp;
    return 0;
}
