#ifndef IO_OPERATIONS_H
#define IO_OPERATIONS_H

#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <stdbool.h>
#include <string.h>
#include "../entropy/entropy.h"

// Status -1 means error.
struct Handler {
    size_t len;
    float *data;
    int32_t status;
};

enum Language
{
    C,
    ASM
};

enum Mode
{
    SCALAR,
    SIMD
};

enum Implementation
{
    DEG2,
    DEG4,
    ARTANH,
    LOOKUP,
    LOG2F
};

struct Handler handle_file(const char *file_name);
float calculate_entropy(size_t n, const float *data, enum Language lan, enum Mode mode, enum Implementation impl);
void print_entropy(enum Language lan, enum Mode mode, enum Implementation impl, float entropy);
void print_mistake(float entropy, double precise_entropy);
void evaluate_args(size_t n, const float *data, enum Language lan, enum Mode mode,
                   enum Implementation impl, double precise_entropy, bool accuracy, bool time, size_t iterations);
void run_full(size_t n, const float *data, double precise_entropy, bool accuracy, bool time, size_t iterations);
void print_usage();

#endif
