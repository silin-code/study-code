#include <iostream>
#include <pthread.h>
using namespace std;

pthread_mutex_t lk;
pthread_cond_t cv;
int is_A_turn = 1;
int print_round = 0;
const int TOTAL_ROUNDS = 10;

void *PrintA(void *args)
{
    while (1)
    {
        pthread_mutex_lock(&lk);
        while (!is_A_turn && print_round < TOTAL_ROUNDS)
        {
            pthread_cond_wait(&cv, &lk);
        }
        if (print_round >= TOTAL_ROUNDS)
        {
            pthread_cond_signal(&cv);
            pthread_mutex_unlock(&lk);
            break;
        }
        cout << "I am A" << endl;
        print_round++;
        is_A_turn = 0;
        pthread_cond_signal(&cv);

        pthread_mutex_unlock(&lk);
    }
    return nullptr;
}

void *PrintB(void *args)
{
    while (1)
    {
        pthread_mutex_lock(&lk);
        while (is_A_turn && print_round < TOTAL_ROUNDS)
        {
            pthread_cond_wait(&cv, &lk);
        }
        if (print_round >= TOTAL_ROUNDS)
        {
            pthread_cond_signal(&cv);
            pthread_mutex_unlock(&lk);
            break;
        }
        cout << "I am B" << endl;
        print_round++;
        is_A_turn = 1;
        pthread_cond_signal(&cv);

        pthread_mutex_unlock(&lk);
    }
    return nullptr;
}

int main()
{
    pthread_t A, B;
    pthread_create(&A, nullptr, PrintA, nullptr);
    pthread_create(&B, nullptr, PrintB, nullptr);

    pthread_join(A, nullptr);
    pthread_join(B, nullptr);

    pthread_mutex_destroy(&lk);
    pthread_cond_destroy(&cv);
    cout << "END" << endl;
    return 0;
}