#ifndef ENTROPY_H
#define ENTROPY_H

#include <stddef.h>
#include <math.h>
#include <immintrin.h>
#include <time.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>

#define likely(x)       __builtin_expect((x),1)
#define unlikely(x)     __builtin_expect((x),0)

// Error Margin
#define UPPER_LIMIT 1.000005f
#define LOWER_LIMIT 0.999995f

#include "io_operations.h"
#include "log2.h"

//===============C functions===================
float c_entropy_scalar(size_t len, const float* data, float (* log2_func) (float));
float c_entropy_simd(size_t len, const float* data, __m128(* log2_func) (__m128));
double c_entropy_precise(size_t len, const float* data);

//===============Asm Functions==================
extern float asm_entropy_scalar (size_t len, const float* data, float (* log2_func) (float));
extern float asm_entropy_simd (size_t len, const float* data, __m128(* log_func) (__m128));

//===============Rand Functions===========
float* c_entropy_rand(size_t len);
float* c_entropy_urandom(size_t len);
float* c_entropy_rand_non_uniform(size_t len);
float* c_entropy_urandom_non_uniform(size_t len);
#endif
