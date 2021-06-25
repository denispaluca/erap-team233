#ifndef ENTROPY_H
#define ENTROPY_H

#include <stddef.h>
#include <math.h>
#include <immintrin.h>
#include <time.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>

#define likely(x) __builtin_expect((x), 1)
#define unlikely(x) __builtin_expect((x), 0)

#include "../io/io_operations.h"
#include "../log2f/log2.h"

//===============C functions===================
float entropy_scalar(size_t len, const float *data, float (*log2_func)(float));
float entropy_simd(size_t len, const float *data, __m128 (*log2_func)(__m128));
double entropy_precise(size_t len, const float *data);

//===============Asm Functions==================
extern float entropy_scalar_asm(size_t len, const float *data, float (*log2_func)(float));
extern float entropy_simd_asm(size_t len, const float *data, __m128 (*log_func)(__m128));

//===============Rand Functions===========
float *entropy_rand(size_t len);
float *entropy_urandom(size_t len);
float *entropy_rand_non_uniform(size_t len);
float *entropy_urandom_non_uniform(size_t len);
#endif
