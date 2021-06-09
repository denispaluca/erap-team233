#ifndef LOG2_H
#define LOG2_H

#include <stdint.h>
#include <immintrin.h>
#include <math.h>

union num { float flt; int fix; };

//===========C Functions==========
float log2approx_deg2(float);
float log2approx_deg4(float);
float log2approx_arctanh(float);
__m128 log2deg2_sse(__m128);
__m128 log2deg4_sse(__m128);
__m128 log2arctanh_sse(__m128);
void init_table(unsigned n, float* table);
float log2_lookup(unsigned n, float* table, float x);
__m128 log2_lookup_simd(unsigned n, float* table, __m128 x);

//===========Asm Functions=========
float log2approx_deg2_asm(float);
float log2approx_deg4_asm(float);
float log2approx_arctanh_asm(float);
__m128 log2deg2_sse_asm(__m128);
__m128 log2deg4_sse_asm(__m128);
__m128 log2arctanh_sse_asm(__m128);
float log2_lookup_asm(unsigned n, float* table, float x);
__m128 log2_lookup_simd_asm(unsigned n, float* table, __m128 x);

#endif