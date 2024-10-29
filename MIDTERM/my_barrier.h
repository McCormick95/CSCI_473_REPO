#ifndef MY_BARRIER_H
#define MY_BARRIER_H

#define PTHREAD_BARRIER_SERIAL_THREAD   1

typedef struct pthread_barrier {
    pthread_mutex_t         mutex;
    pthread_cond_t          cond;
    volatile uint32_t       flag;
    size_t                  count;
    size_t                  num;
} pthread_barrier_t;

int pthread_barrier_init(pthread_barrier_t *bar, int attr, int num);

int pthread_barrier_wait(pthread_barrier_t *bar);

#endif