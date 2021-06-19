#ifndef LOG2_H
#define LOG2_H

#include <stdint.h>
#include <immintrin.h>
#include <math.h>

union num { float flt; uint32_t fix; };
union num_s { __m128 flt; __m128i fix; };

// Constants
// Scalar
extern const float deg2_consts[];
extern const float deg4_consts[];
extern const float artanh_consts[];
// SIMD
extern const uint32_t reduce_mask[];
extern const uint32_t mantissa_mask[];
extern const uint32_t f_bias[];
extern const uint32_t normalize_mask[];
extern const uint32_t normalize_exp[];
extern const float normalize_const[];
extern const float f_one[];
extern const float one_third[];
extern const float one_fifth[];
extern const float ln2_inverse_2[];
extern const float deg2_co1[];
extern const float deg2_co2[];
extern const float deg2_co3[];
extern const float deg4_co1[];
extern const float deg4_co2[];
extern const float deg4_co3[];
extern const float deg4_co4[];
extern const float deg4_co5[];
extern const float log_lookup_table[];

// Lookup table size
#define LOG_LOOKUP_TABLE_SIZE 14


//===========C Functions==========
float log2approx_deg2(float);
float log2approx_deg4(float);
float log2approx_arctanh(float);
__m128 log2approx_deg2_simd(__m128);
__m128 log2approx_deg4_simd(__m128);
__m128 log2approx_arctanh_simd(__m128);
void init_table(unsigned n, float* table);
float log2_lookup(float x);
__m128 log2_lookup_simd(__m128 x);

//===========Asm Functions=========
float log2approx_deg2_asm(float);
float log2approx_deg4_asm(float);
float log2approx_arctanh_asm(float);
__m128 log2approx_deg2_simd_asm(__m128);
__m128 log2approx_deg4_simd_asm(__m128);
__m128 log2approx_arctanh_simd_asm(__m128);
float log2_lookup_asm(float x);
__m128 log2_lookup_simd_asm(__m128 x);

#endif