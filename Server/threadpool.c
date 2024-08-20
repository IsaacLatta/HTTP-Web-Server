#include "threadpool.h"

threadpool_args_t* _threadpool_args_create(void* arg1, void* arg2, void* arg3)
{
    threadpool_args_t* args = (threadpool_args_t*)malloc(sizeof(threadpool_args_t));
    if(!args)
    {
        return NULL;
    }
    args->arg1 = arg1;
    args->arg2 = arg2;
    args->arg3 = arg3;
    return args;
}

node_t* queue_create_node(void* data)
{
    node_t* node = (node_t*)malloc(sizeof(node_t));
    if(!node)
    {
        return NULL;
    } 
    node->data = data;
    node->next = NULL;
    return node;
}

queue_t* queue_create()
{
    queue_t* queue = (queue_t*)malloc(sizeof(queue_t));
    if(!queue)
    {
        return NULL;
    }
    queue->size = 0;
    queue->front = NULL;
    queue->back = NULL;
    pthread_mutex_init(&queue->lock, NULL);
    pthread_cond_init(&queue->notify, NULL);
    return queue;
}

int queue_size(queue_t* queue)
{
    pthread_mutex_lock(&queue->lock);
    int qsize = queue->size;
    pthread_mutex_unlock(&queue->lock);
    return qsize;
}

void queue_push(queue_t* queue, void* data)
{
    node_t* node = queue_create_node(data);
    if(!node)
    {
        return;
    }

    pthread_mutex_lock(&queue->lock);

    if(queue->back == NULL)
    {
        queue->back = node;
        queue->front = node;
    }
    else
    {
        queue->back->next = node; // Link the old back's next node to new node
        queue->back = node; // Assign the new back node to node
    }

    queue->size++;
    pthread_mutex_unlock(&queue->lock);
    pthread_cond_signal(&queue->notify);
}

void* queue_pop(queue_t* queue)
{
    pthread_mutex_lock(&queue->lock);
    
    while(queue->front == NULL && !queue->close) // Wait for new task
    {
        pthread_cond_wait(&queue->notify, &queue->lock);
    }
    if(queue->close && queue->size == 0)
    {
        pthread_mutex_unlock(&queue->lock);
        return NULL;
    }
    
    node_t* old_node = queue->front;
    void* data = old_node->data;
    queue->front = queue->front->next;

    if(queue->front == NULL) // queue is now empty
    {
        queue->back = NULL;
    }

    queue->size--;
    pthread_mutex_unlock(&queue->lock);
    return data; 
}

void queue_destroy(queue_t* queue)
{
    pthread_mutex_lock(&queue->lock);
    queue->close = 1;
    pthread_mutex_unlock(&queue->lock);
    pthread_cond_broadcast(&queue->notify);

    pthread_mutex_lock(&queue->lock); // retreive lock after all queue_pop calls exit
    node_t* current_node = queue->front;
    node_t* next_node;

    while(current_node != NULL) 
    {
        next_node = current_node->next;
        if(current_node->data != NULL)
        {
            free(current_node->data);
        }
        free(current_node);
        current_node = next_node; 
    }

    pthread_mutex_unlock(&queue->lock);
    pthread_mutex_destroy(&queue->lock);
    pthread_cond_destroy(&queue->notify);
    free(queue);
}

int threadpool_push(threadpool_t* threadpool, void (*function)(void*), void* arg1, void* arg2, void* arg3)
{
    threadpool_task_t* task = (threadpool_task_t*)malloc(sizeof(threadpool_task_t));
    if(task == NULL)
    {
        return -1;
    }
    task->function = function;
    task->args = _threadpool_args_create(arg1, arg2, arg3);
    if(task->args == NULL)
    {
        return -1;
    }
    queue_push(threadpool->queue, task);
    return 0;
}

void* _threadpool_worker(void* arg)
{
    threadpool_t* threadpool = (threadpool_t*)arg;
    threadpool_task_t* task;

    sigset_t set;
    sigemptyset(&set);
    sigaddset(&set, SIGINT);
    sigaddset(&set, SIGTERM);
    pthread_sigmask(SIG_BLOCK, &set, NULL); // Block signals in child threads
    
    while(!threadpool->quit)
    {
        task = (threadpool_task_t*)queue_pop(threadpool->queue);
        if(task == NULL) // queue is closing
        {
            break;
        }
        (*(task->function))(task->args); // execute the task
        free(task->args);
        free(task);
    }
    return NULL;
}

threadpool_t* threadpool_create(int thread_count)
{
    threadpool_t* threadpool = (threadpool_t*)malloc(sizeof(threadpool_t));
    if(!threadpool)
    {
        return NULL;
    }
    threadpool->threads = (pthread_t*)malloc(sizeof(pthread_t)*thread_count);
    if(!threadpool->threads)
    {
        free(threadpool);
        return NULL;
    }
    threadpool->queue = queue_create();
    if(!threadpool->queue)
    {
        free(threadpool->threads);
        free(threadpool);
        return NULL;
    }

    threadpool->thread_count = thread_count;
    threadpool->quit = 0;

    for(int i = 0; i < thread_count; i++)
    {
        if(pthread_create(&threadpool->threads[i], NULL, _threadpool_worker, (void*)threadpool) != 0)
        {
            threadpool_destroy(threadpool);
            return NULL;
        }
    }
    return threadpool;
}

void threadpool_destroy(threadpool_t* threadpool)
{
    if(threadpool == NULL)
    {
        return;
    }
    threadpool->quit = 1;
    queue_destroy(threadpool->queue);
    for(int i = 0; i < threadpool->thread_count; i++)
    {
        pthread_join(threadpool->threads[i], NULL);
    }
    free(threadpool);
}


