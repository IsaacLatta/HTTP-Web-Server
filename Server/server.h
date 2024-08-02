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

#define ERROR 42
#define LOG 44

typedef struct
{
    int listenfd;
    struct sockaddr_in addr;
} server_t;

void _log(int type, char* s1, char* s2, int sockfd);
void server_run(server_t* server, char* root_dir, int port);


#endif