// Itamar Laredo
#include <stdio.h>
#include <pthread.h>
#include <stdlib.h>
#include <string.h>
#include "threadPool.h"


void terminate(ThreadPool *tp, char* error);

/*
 * Memory free function.
 * Initialize task and dequeue all the task's the queue contain
 * and free it all.
 * Free any other object that got memory allocation with malloc.
 */
void memfree(ThreadPool* tp) {
    Task *task;
    while (!osIsQueueEmpty(tp->osQueue)) {
        task = osDequeue(tp->osQueue);
        free(task);
    }

    osDestroyQueue(tp->osQueue);
    tp->osQueue = NULL;
    free(tp->mutex);
    free(tp->cond);
    free(tp->threads_arr);
    free(tp);
}

/*
 * Terminate function.
 * Printing one-line informative error comment, free the program memory and exit.
 */
void terminate(ThreadPool *tp, char* error) {
    perror(error);
    memfree(tp);
    exit(-1);
}

/*
 * Thread executing function.
 * If the queue is empty, should wait for task. Otherwise, dequeue
 * one task and execute it.
 */
void* thread_func(void* f) {
    ThreadPool* tp = (ThreadPool*) f;
    Task *task;
    while(tp->waitForAllTasks) {
        if (!tp->canInsert && osIsQueueEmpty(tp->osQueue))
            break;
        if (osIsQueueEmpty(tp->osQueue)) {
            if (pthread_mutex_lock(tp->mutex) != 0)
                terminate(tp, "Error in pthread_mutex_lock");
            if (pthread_cond_wait(tp->cond, tp->mutex) != 0)
                terminate(tp, "Error in pthread_cond_wait");
            if (pthread_mutex_unlock(tp->mutex) != 0)
                terminate(tp, "Error in pthread_mutex_unlock");
        } else {
            if (pthread_mutex_lock(tp->mutex) != 0)
                terminate(tp, "Error in pthread_mutex_lock");
            task = osDequeue(tp->osQueue);
            if (pthread_mutex_unlock(tp->mutex) != 0)
                terminate(tp, "Error in pthread_mutex_unlock");
            if (task != NULL) {
                ((task->computeFunc))(task->param);
                free(task);
            }
        }
    }
}

/*
 * Create Thread Pool function.
 * Initialize tp fields and allocating memory.
 * Initialize threads array inorder to store the threads in data structure.
 */
ThreadPool* tpCreate(int numOfThreads) {
    int i;
    ThreadPool *tp = (ThreadPool *) malloc(sizeof(ThreadPool));
    if (!tp)
        terminate(tp, "Error in memory allocation");
    tp->thread_amount = numOfThreads;
    tp->osQueue = osCreateQueue();
    tp->waitForAllTasks = 1;
    tp->canInsert = 1;

    tp->threads_arr = (pthread_t *) malloc(sizeof(pthread_t) * numOfThreads);
    if (!tp->threads_arr)
        terminate(tp, "Error in memory allocation");

    tp->cond = (pthread_cond_t *) malloc (sizeof(pthread_cond_t));
    if (!tp->cond)
        terminate(tp, "Error in memory allocation");

    tp->mutex = (pthread_mutex_t *) malloc (sizeof(pthread_mutex_t));
    if (!tp->mutex)
        terminate(tp, "Error in memory allocation");

    if (pthread_cond_init(tp->cond, NULL) != 0)
        terminate(tp, "Error in pthread_cond_init");

    if (pthread_mutex_init(tp->mutex, NULL) != 0)
        terminate(tp, "Error in pthread_mutex_init");

    for(i = 0; i < numOfThreads; i++) {
        if (pthread_create(&(tp->threads_arr[i]), NULL, thread_func, (void *) tp) != 0)
            terminate(tp, "Error in pthread_create");
    }
    return tp;
}

/*
 * Destroy the Thread Pool function.
 * First turn the option to insert new tasks to the queue to unavailable.
 * Then, if not should wait for tasks marked it. Wakes up all threads that
 * are currently waiting and terminate them.
 */
void tpDestroy(ThreadPool* threadPool, int shouldWaitForTasks) {
    int i;
    if (shouldWaitForTasks != 0)
        threadPool->canInsert = 0;
    else {
        threadPool->canInsert = 0;
        threadPool->waitForAllTasks = 0;
    }

    if (pthread_cond_broadcast(threadPool->cond) != 0)
        terminate(threadPool, "Error in pthread_cond_broadcast");

    for (i = 0; i < threadPool->thread_amount; i++) {
        if (pthread_join(threadPool->threads_arr[i], NULL) != 0)
            terminate(threadPool, "Error in pthread_join");
    }
    memfree(threadPool);
}

/*
 * Thread Pool Insert Task function.
 * If possible to insert new task to the queue, initialize new task with the
 * executed function and param given. Wake up at least one thread that is
 * currently waiting. If not possible free the memory and return error.
 */
int tpInsertTask(ThreadPool* threadPool, void (*computeFunc) (void *), void* param) {
    if (threadPool->canInsert) {
        Task *task = (Task *) malloc(sizeof(Task));
        if (!task)
            terminate(threadPool, "Error in memory allocation");

        if (pthread_mutex_lock(threadPool->mutex) != 0)
            terminate(threadPool, "Error in pthread_mutex_lock");
        task->computeFunc = computeFunc;
        task->param = param;
        osEnqueue(threadPool->osQueue, task);
        if (pthread_mutex_unlock(threadPool->mutex) != 0)
            terminate(threadPool, "Error in pthread_mutex_unlock");

        if (pthread_cond_signal(threadPool->cond) != 0)
            terminate(threadPool, "Error in pthread_cond_signal");
        return 0;
    } else {
        memfree(threadPool);
        return -1;
    }
}
