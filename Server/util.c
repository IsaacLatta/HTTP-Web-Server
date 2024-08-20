#include "util.h"

void print_exit(char* msg)
{
    printf("%s\n", msg);
    exit(EXIT_SUCCESS);
}

void pexit(char* error_msg)
{
    char buffer[512];
    sprintf(buffer, "ERROR: %s, Errno=%d\n", error_msg, errno);
    perror(buffer);
    exit(EXIT_FAILURE);
}

void* ec_malloc(size_t size)
{
    void* data = malloc(size);
    if(!data)
    {
        pexit("malloc failed");
    }
    return data;
}

void sleep_ms(int ms)
{
    struct timeval tv;
    tv.tv_sec = ms / 1000;
    tv.tv_usec = (ms % 1000)*1000;
    select(0, NULL, NULL, NULL, &tv);
}
