#include<stdio.h>
#include<stdlib.h>
#include<pthread.h>
#include<semaphore.h>

pthread_mutex_t mutex;
pthread_cond_t full, empty;
sem_t x;


void *producer(void* arg)
{
    pthread_mutex_lock(&mutex);
    sem_wait(&x);
    sem_post(&x);

    pthread_cond_wait(&full, &mutex);
    pthread_cond_signal(&full);

    pthread_mutex_unlock(&mutex);
    
}

int main()
{

    pthread_t prod;
    pthread_mutex_init(&mutex, NULL);
    pthread_cond_init(&full, NULL);
    pthread_create(&prod, NULL, producer, NULL);
    pthread_join(prod, NULL);
    pthread_mutex_destroy(&mutex);
    pthread_cond_destroy(&full);

    sem_init(&x, 0, 10);
    sem_destroy(&x);
    

    return 0;
}