#ifndef IO_OPERATIONS_H
#define IO_OPERATIONS_H

#include <stdio.h>
#include <stdlib.h>

// Error Margin
#define UPPER_LIMIT 1.000005
#define LOWER_LIMIT 0.999995

int size_file(const char* file_name);
float* read_file(size_t len, const char* file_name);

#endif
