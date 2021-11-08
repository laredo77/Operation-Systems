#ifndef __THREAD_POOL__
#define __THREAD_POOL__

#include "osqueue.h"
#include <unistd.h>

typedef struct task {
    void (*computeFunc) (void *);
    void* param;
} Task;

typedef struct thread_pool {
    int waitForAllTasks;
    int canInsert;
    int thread_amount;
    OSQueue *osQueue;
    pthread_t *threads_arr;
    pthread_cond_t* cond;
    pthread_mutex_t* mutex;
} ThreadPool;

ThreadPool* tpCreate(int numOfThreads);

void tpDestroy(ThreadPool* threadPool, int shouldWaitForTasks);

int tpInsertTask(ThreadPool* threadPool, void (*computeFunc) (void *), void* param);

#endif
