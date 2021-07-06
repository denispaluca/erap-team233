#ifndef LOG2_H
#define LOG2_H

#include <stdint.h>
#include <immintrin.h>
#include <math.h>

// Lookup table size
#define LOG_LOOKUP_TABLE_SIZE 16

// glibc implementation constants
#define GLIBC_TABLE_BITS = 4
#define GLIBC_POLY_ORDER = 4

union num
{
    float flt;
    uint32_t fix;
};
union num_s
{
    __m128 flt;
    __m128i fix;
};

// Constants
// Scalar
extern const float deg2_consts[];
extern const float deg4_consts[];
extern const float artanh_consts[];

// SIMD
extern const __m128i reduce_mask;
extern const __m128i mantissa_mask;
extern const __m128i bias_packed;
extern const __m128i normalize_mask;
extern const __m128i normalize_exp;

extern const __m128 normalize_const;
extern const __m128 one_packed;
extern const __m128 one_third;
extern const __m128 one_fifth;
extern const __m128 ln2_inverse_2;

extern const __m128 deg2_co1;
extern const __m128 deg2_co2;
extern const __m128 deg2_co3;

extern const __m128 deg4_co1;
extern const __m128 deg4_co2;
extern const __m128 deg4_co3;
extern const __m128 deg4_co4;
extern const __m128 deg4_co5;

extern const __m128i glibc_off;
extern const __m128i glibc_extract;
extern const __m128i glibc_mod16_mask;
extern const float glibc_inverse_c[];
extern const float glibc_logc[];
extern const __m128 glibc_co0;
extern const __m128 glibc_co1;
extern const __m128 glibc_co2;
extern const __m128 glibc_co3;

extern const float log_lookup_table[];

//===========C Functions==========
float log2_deg2_scalar(float);
float log2_deg4_scalar(float);
float log2_artanh_scalar(float);
__m128 log2_deg2_simd(__m128);
__m128 log2_deg4_simd(__m128);
__m128 log2_artanh_simd(__m128);
void init_table(unsigned n, float *table);
float log2_lookup_scalar(float x);
__m128 log2_lookup_simd(__m128 x);

//===========Asm Functions=========
float log2_deg2_scalar_asm(float);
float log2_deg4_scalar_asm(float);
float log2_artanh_scalar_asm(float);
__m128 log2_deg2_simd_asm(__m128);
__m128 log2_deg4_simd_asm(__m128);
__m128 log2_artanh_simd_asm(__m128);
float log2_lookup_scalar_asm(float x);
__m128 log2_lookup_simd_asm(__m128 x);
__m128 log2_glibc_simd(__m128 x);

#endif
