#include<stdio.h>
#include<stdlib.h>
#include<string.h>
#include<pthread.h>

pthread_mutex_t m, n, o;
pthread_cond_t a, b, c;

void* thread1(void* arg)
{
    while(1)
    {

        pthread_mutex_lock(&m);
        printf("I am thread 0\n");
        pthread_cond_signal(&b);
        pthread_cond_wait(&a, &m);
        pthread_mutex_unlock(&m);
    }    

}

void* thread2(void* arg)
{
    pthread_mutex_lock(&n);
    pthread_cond_wait(&b, &n);
    pthread_mutex_unlock(&n);

    while(1)
    {

        pthread_mutex_lock(&n);
        printf("I am thread 1\n");
        pthread_cond_signal(&c);
        pthread_cond_wait(&b, &n);
        pthread_mutex_unlock(&n);
    }   
}

void* thread3(void* arg)
{
    pthread_mutex_lock(&o);
    pthread_cond_wait(&c, &o);
    pthread_mutex_unlock(&o);
    while(1)
    {

        pthread_mutex_lock(&o);
        printf("I am thread 2\n");
        pthread_cond_signal(&a);
        pthread_cond_wait(&c, &o);
        pthread_mutex_unlock(&o);
    }   
}

int main()
{
    pthread_t x, y, z;
    pthread_mutex_init(&m, NULL);
    pthread_mutex_init(&n, NULL);
    pthread_mutex_init(&o, NULL);
    pthread_cond_init(&a, NULL);
    pthread_cond_init(&b, NULL);
    pthread_cond_init(&c, NULL);
    
    pthread_create(&x, NULL, thread1, NULL);
    pthread_create(&y, NULL, thread2, NULL);
    pthread_create(&z, NULL, thread3, NULL);

    pthread_join(x, NULL);
    pthread_join(y, NULL);
    pthread_join(z, NULL);

    pthread_mutex_destroy(&m);
    pthread_mutex_destroy(&n);
    pthread_mutex_destroy(&o);
    pthread_cond_destroy(&a);
    pthread_cond_destroy(&b);
    pthread_cond_destroy(&c);

    return 0;
}