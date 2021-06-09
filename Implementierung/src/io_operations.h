#ifndef IO_OPERATIONS_H
#define IO_OPERATIONS_H

#include <stdio.h>
#include <stdlib.h>

// Error Margin
#define UPPER_LIMIT 1.000005f
#define LOWER_LIMIT 0.999995f

int size_file(const char* file_name);
float* read_file(size_t len, const char* file_name);

#endif
