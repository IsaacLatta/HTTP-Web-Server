#ifndef THREADPOOL_H
#define THREADPOOL_H

#define _POSIX_C_SOURCE 200809L

#include <stdlib.h>
#include <stdio.h>
#include <pthread.h>
#include <unistd.h> 
#include <signal.h>

typedef struct
{
    void* arg1;
    void* arg2;
    void* arg3;
} threadpool_args_t;

typedef struct
{
    void (*function)(void*);
    threadpool_args_t* args;
} threadpool_task_t;

typedef struct Node
{
    void* data;
    struct Node* next;
} node_t;

typedef struct 
{
    node_t* front;
    node_t* back;
    pthread_mutex_t lock;
    pthread_cond_t notify;
    int close;
    int size;
} queue_t;

typedef struct
{
    pthread_t* threads;
    queue_t* queue;
    int thread_count;
    int quit;
} threadpool_t;

threadpool_t* threadpool_create(int thread_count);
int threadpool_push(threadpool_t* threadpool, void(*function)(void*), void* arg1, void* arg2, void* arg3);
void threadpool_destroy(threadpool_t* threadpool);

#endif