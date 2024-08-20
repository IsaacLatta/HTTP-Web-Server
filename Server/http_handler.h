#ifndef HTTP_HANDLER_H
#define HTTP_HANDLER_H

#include <stdlib.h>
#include <string.h>
#include <fcntl.h>
#include <errno.h>
#include <stdio.h>
#include <unistd.h>
#include <pthread.h>

#include "util.h"

#define SUCCESS 0
#define ERROR 42
#define LOG 44
#define BAD_REQUEST 400
#define FORBIDDEN 403
#define NOT_FOUND 404
#define INTERNAL_SERVER_ERROR 500

#define BUFFER_SIZE 65536

typedef struct 
{
    int sockfd;
    int hit;
    char* resource;
    char* method;
    char* content_type;
} request_t;

void handle_request(char* buffer, int sockfd, int hit);
void logger(int type, char* s1, char* s2, int sockfd);

#endif