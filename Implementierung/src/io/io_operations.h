#ifndef IO_OPERATIONS_H
#define IO_OPERATIONS_H

#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
// Status -1 means error.
struct Handler
{
    size_t len;
    float *data;
    int32_t status;
};

struct Handler handle_file(const char *file_name);

#endif
