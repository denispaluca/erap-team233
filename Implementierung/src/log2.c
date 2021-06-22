#include "log2.h"

inline void reduce_float(union num *data, int *exponent)
{

    // Get exponent (omitting sign bit()
    *exponent = data->fix >> 23;

    // Special case for denormal floating numbers
    if (*exponent == 0)
    {
        data->flt *= normalize_const[0];    /* Normalizefloating number */
        *exponent = (data->fix >> 23) - 23; /* Recalculate exponent considering exponent used for normalization*/
    }

    // Subtracting bias
    *exponent -= f_bias[0];

    data->fix = (data->fix & mantissa_mask[0]) | reduce_mask[0];
}

// Normal
float log2approx_deg2(float x)
{
    union num data = {.flt = x};
    int exponent;

    reduce_float(&data, &exponent);

    // co1 * x^2 + co2 * x + co3

    // Pipelining
    float y0, y;
    y0 = deg2_co3[0] + exponent;

    y = deg2_co1[0] * data.flt + deg2_co2[0];

    return y * data.flt + y0;
}

float log2approx_deg4(float x)
{
    union num data = {.flt = x};
    int exponent;

    reduce_float(&data, &exponent);

    // co1 * x^4 + co2 * x^3 + co3 * x^2 + co2 * x + co1

    // Pipelining
    float y0, x2, y, z;
    y0 = deg4_co5[0] + exponent;

    x2 = data.flt * data.flt;

    y = deg4_co2[0] * data.flt + deg4_co3[0];
    y = deg4_co1[0] * x2 + y;
    z = deg4_co4[0] * data.flt + y0;

    return y * x2 + z;
}

float log2approx_arctanh(float x)
{
    union num data = {.flt = x};
    int exponent;

    reduce_float(&data, &exponent);

    // q = (x - 1) / (x + 1)
    // ( 0.2 * q^5 + 0.3 * q^3 + q ) * (2 / ln2)

    // Pipelining
    float q, q2, y;

    q = (data.flt - 1) / (data.flt + 1);
    q2 = q * q;

    y = one_third[0] + q2 * one_fifth[0];
    y = y * q2 + 1;
    y = y * q * ln2_inverse_2[0];

    return y + exponent;
}

inline void reduce_float_simd(union num_s *data, __m128i *exponent)
{

    /* 
    1.0f:       0 01111111 00000000000000000000000
    2^23f:      0 10010110 00000000000000000000000
    xor mask:   0 11101001 00000000000000000000000
    zero mask   0 00000000 00000000000000000000000

    For exponent recalcultaion easily recalculate exponent for all and subtraact 23 for denormalized ones

    so that you can easily multiply by 2^23f
    */

    // Get exponent (omitting sign bit)
    *exponent = _mm_srli_epi32(data->fix, 23);

    // Special case for denormal floating numbers
    union num_s mask;
    mask.fix = _mm_cmpeq_epi32(*exponent, _mm_setzero_si128());
    __m128i exp_fix = _mm_load_si128((const __m128i *)normalize_exp) & mask.fix;
    mask.fix &= _mm_load_si128((const __m128i *)normalize_mask);
    mask.fix ^= _mm_loadu_si128((const __m128i *)f_one);

    data->flt *= mask.flt;

    *exponent = _mm_srli_epi32(data->fix, 23);
    *exponent = _mm_sub_epi32(*exponent, _mm_load_si128((const __m128i *)f_bias));
    *exponent = _mm_sub_epi32(*exponent, exp_fix);

    data->fix = (data->fix & _mm_load_si128((const __m128i *)mantissa_mask)) | _mm_load_si128((const __m128i *)reduce_mask);
}

// SIMD
__m128 log2approx_deg2_simd(__m128 x)
{
    union num_s data = {.flt = x};
    __m128i exponent;

    reduce_float_simd(&data, &exponent);

    // Pipelining
    __m128 y0, y;
    y0 = _mm_load_ps(deg2_co3) + _mm_cvtepi32_ps(exponent);

    y = _mm_load_ps(deg2_co1) * data.flt + _mm_load_ps(deg2_co2);

    return y * data.flt + y0;
}

__m128 log2approx_deg4_simd(__m128 x)
{
    union num_s data = {.flt = x};
    __m128i exponent;

    reduce_float_simd(&data, &exponent);

    // Pipelining
    __m128 y0, x2, y, z;
    y0 = _mm_load_ps(deg4_co5) + _mm_cvtepi32_ps(exponent);

    x2 = data.flt * data.flt;

    y = _mm_load_ps(deg4_co2) * data.flt + _mm_load_ps(deg4_co3);
    y = _mm_load_ps(deg4_co1) * x2 + y;
    z = _mm_load_ps(deg4_co4) * data.flt + y0;

    return y * x2 + z;
}

__m128 log2approx_arctanh_simd(__m128 x)
{
    union num_s data = {.flt = x};
    __m128i exponent;

    reduce_float_simd(&data, &exponent);

    // Pipelining
    __m128 q, q2, y;

    q = (data.flt - _mm_load_ps(f_one)) / (data.flt + _mm_load_ps(f_one));
    q2 = q * q;

    y = _mm_load_ps(one_third) + q2 * _mm_load_ps(one_fifth);
    y = y * q2 + _mm_load_ps(f_one);
    y = y * q * _mm_load_ps(ln2_inverse_2);

    return y + _mm_cvtepi32_ps(exponent);
}

// With Lookup table

float log2_lookup(float x)
{
    union num data = {.flt = x};
    int exponent, index;

    reduce_float(&data, &exponent);

    index = (data.fix & mantissa_mask[0]) >> (23 - LOG_LOOKUP_TABLE_SIZE);

    return log_lookup_table[index] + exponent;
}

__m128 log2_lookup_simd(__m128 x)
{
    union num_s data = {.flt = x};
    __m128i exponent, index;

    reduce_float_simd(&data, &exponent);

    index = _mm_srli_epi32(data.fix & _mm_load_si128((const __m128i *)mantissa_mask), (23 - LOG_LOOKUP_TABLE_SIZE));

    __m128 y = _mm_set_ps(
        log_lookup_table[((__v4si)index)[3]],
        log_lookup_table[((__v4si)index)[2]],
        log_lookup_table[((__v4si)index)[1]],
        log_lookup_table[((__v4si)index)[0]]);

    return y + _mm_cvtepi32_ps(exponent);
}

//TODO: Implement
__m128 log2_glibc_simd(__m128 x)
{
    
}
