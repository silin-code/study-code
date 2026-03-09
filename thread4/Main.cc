#include "RingQueue.hpp"

Mutex screen_lock;

void Print(const std::string &name, const std::string &info)
{
    screen_lock.Lock();
    std::cout << name << ":" << info << std::endl;
    screen_lock.Unlock();
}

class ThreadData
{
public:
    ThreadData(RingQueue<int> *r, const std::string &n)
        : rq(r), name(n)
    {
    }
    ~ThreadData() {}

public:
    std::string name;
    RingQueue<int> *rq;
};

void *Productor(void *args)
{
    ThreadData *td = static_cast<ThreadData *>(args);
    pthread_setname_np(pthread_self(), td->name.c_str());
    int data = 1;
    while (1)
    {
        td->rq->Enqueue(data);
        data++;
        Print(td->name,"生产"+std::to_string(data));
    }
}

void *Consumer(void *args)
{
    ThreadData *td = static_cast<ThreadData *>(args);
    pthread_setname_np(pthread_self(), td->name.c_str());
    int data = 0;
    while (1)
    {
        sleep(2);
        td->rq->Pop(&data);
        Print(td->name,"消费"+std::to_string(data));
    }
}

int main()
{
    RingQueue<int> *rq = new RingQueue<int>();
    pthread_t c[2], p[3];

    ThreadData *td1 = new ThreadData(rq, "Product-1");
    ThreadData *td2 = new ThreadData(rq, "Product-2");
    ThreadData *td3 = new ThreadData(rq, "Product-3");
    ThreadData *td4 = new ThreadData(rq, "Consumer-1");
    ThreadData *td5 = new ThreadData(rq, "Consumer-2");
    pthread_create(&p[0], nullptr, Productor, td1);
    pthread_create(&p[1], nullptr, Productor, td2);
    pthread_create(&p[2], nullptr, Productor, td3);
    pthread_create(&c[0], nullptr, Consumer, td4);
    pthread_create(&c[1], nullptr, Consumer, td5);

    pthread_join(c[0], nullptr);
    pthread_join(c[1], nullptr);
    pthread_join(p[0], nullptr);
    pthread_join(p[1], nullptr);
    pthread_join(p[2], nullptr);
}