#ifndef ENTROPY_H
#define ENTROPY_H

#include <stddef.h>
#include <math.h>
#include <immintrin.h>

#include "io_operations.h"

//===============C functions===================
float scalar_entropy(size_t len,  float* data);
float simd_entropy(size_t len, float* data);
float file_entropy_c(const char* file_name);

//===============Asm Functions==================
extern float entropy_asm (size_t len, float* data);
extern float entropy_simd (size_t len, float* data);
extern float log2_asm(float val);
extern __m128 log2_simd(__m128 val);

#endif
