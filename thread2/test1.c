#include <stdio.h>
#include <pthread.h>
#include <unistd.h>

int ticket = 66;

pthread_mutex_t mutex;

int arr[6]={0};

void *grap_ticket(void *arg)
{
    int id = *(int *)arg;

    while (1)
    {
        pthread_mutex_lock(&mutex);

        if (ticket > 0)
        {
            usleep(100000);
            printf("黄牛%d抢到了第%d张票\n", id, ticket);
            ticket--;
            arr[id]++;
        }
        else
        {
            pthread_mutex_unlock(&mutex);
            break;
        }
        pthread_mutex_unlock(&mutex);
        usleep(50000);
    }
    printf("黄牛%d抢完了\n", id);
    pthread_exit(NULL);
}

int main()
{
    pthread_t thread[5];
    int ids[5] = {1, 2, 3, 4, 5};

    pthread_mutex_init(&mutex, NULL);

    for (int i = 0; i < 5; i++)
    {
        pthread_create(&thread[i], NULL, grap_ticket, &ids[i]);
    }

    for (int i = 0; i < 5; i++)
    {
        pthread_join(thread[i], NULL);
    }
    pthread_mutex_destroy(&mutex);
    printf("抢完了，这是成果\n");
    for(int i=1;i<=5;i++)
    {
        printf("%d黄牛,抢到了%d张票\n",i,arr[i]);
    }
    return 0;
}
