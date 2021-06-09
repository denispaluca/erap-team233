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

#include "io_operations.h"
#include "log2.h"

//===============C functions===================
float scalar_entropy(size_t len,  float* data);
float simd_entropy(size_t len, float* data);
float file_entropy_c(const char* file_name);

//===============Asm Functions==================
extern float entropy_asm (size_t len, float* data);
extern float entropy_simd (size_t len, float* data);

//===============Rand Functions===========
float entropy_c_rand(size_t len);
float entropy_c_urandom(size_t len);

#endif
