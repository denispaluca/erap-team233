#ifndef ENTROPY_H
#define ENTROPY_H

#include <stddef.h>
#include <math.h>
#include <immintrin.h>
#include <time.h>
#include <stdio.h>
#include <stdlib.h>

#define likely(x)       __builtin_expect((x),1)
#define unlikely(x)     __builtin_expect((x),0)

// Error Margin
#define UPPER_LIMIT 1.000005f
#define LOWER_LIMIT 0.999995f

#include "io_operations.h"
#include "log2.h"

//===============C functions===================
float scalar_entropy(size_t len, const float* data, float (* log2_func) (float));
float simd_entropy(size_t len, const float* data, __m128(* log2_func) (__m128));
double precise_entropy(size_t len, const float* data);

//===============Asm Functions==================
extern float entropy_asm (size_t len, const float* data, float (* log2_func) (float));
extern float entropy_simd (size_t len, const float* data, __m128(* log_func) (__m128));

//===============Rand Functions===========
float* entropy_c_rand(size_t len);
float* entropy_c_urandom(size_t len);

#endif
