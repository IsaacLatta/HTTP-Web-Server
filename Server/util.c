#include "util.h"

void pexit(char* error_msg)
{
    perror(error_msg);
    exit(EXIT_FAILURE);
}
