#ifndef IO_OPERATIONS_H
#define IO_OPERATIONS_H

#include <stdio.h>
#include <stdlib.h>

struct Handler
{
    size_t len;
    float *data;
    int status;
};
size_t size_file(const char *file_name);
float *read_file(size_t len, const char *file_name);
struct Handler handle_file(const char* file_name);

#endif