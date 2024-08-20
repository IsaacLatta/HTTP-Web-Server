#ifndef SERVER_H
#define SERVER_H

#include "util.h"
#include <string.h>
#include <arpa/inet.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <errno.h>
#include <fcntl.h>
#include <unistd.h>
#include <signal.h>


#include "threadpool.h"
#include "http_handler.h"

typedef struct
{
    threadpool_t* pool;
    int listenfd;
    struct sockaddr_in addr;
} server_t;

void server_run(server_t* server, int port);
void server_worker(void* arg);

#endif