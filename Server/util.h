#ifndef UTIL_H
#define UTIL_H

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/select.h>
#include <errno.h>

#ifdef VERBOSE
    #define VERBOSE_LOG(fmt, args...) fprintf(stdout, "[LOG] " fmt, ##args)
#else
    #define VERBOSE_LOG(fmt, args...)
#endif

void print_exit(char* msg);
void pexit(char* error_msg);
void* ec_malloc(size_t size);
void sleep_ms(int ms);

#endif 