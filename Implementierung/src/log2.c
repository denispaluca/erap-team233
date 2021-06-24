#include "log2.h"

inline void reduce_float_scalar(union num *data, int *exponent)
{

    // Get exponent (omitting sign bit()
    *exponent = data->fix >> 23;

    // Special case for denormal floating numbers
    if (*exponent == 0)
    {
        data->flt *= 0x1P23f;               /* Normalizefloating number */
        *exponent = (data->fix >> 23) - 23; /* Recalculate exponent considering exponent used for normalization*/
    }

    // Subtracting bias
    *exponent -= 127;

    data->fix = (data->fix & 0x7FFFFF) | 0x3F800000;
}

// Normal
float log2_deg2_scalar(float x)
{
    union num data = {.flt = x};
    int exponent;

    reduce_float_scalar(&data, &exponent);

    // co1 * x^2 + co2 * x + co3

    // Pipelining
    float y0, y;
    y0 = deg2_co3[0] + exponent;

    y = deg2_co1[0] * data.flt + deg2_co2[0];

    return y * data.flt + y0;
}

float log2_deg4_scalar(float x)
{
    union num data = {.flt = x};
    int exponent;

    reduce_float_scalar(&data, &exponent);

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

float log2_artanh_scalar(float x)
{
    union num data = {.flt = x};
    int exponent;

    reduce_float_scalar(&data, &exponent);

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
    __m128i exp_fix = normalize_exp & mask.fix;
    mask.fix &= normalize_mask;
    mask.fix ^= (const __m128i)f_one;

    data->flt *= mask.flt;

    *exponent = _mm_srli_epi32(data->fix, 23);
    *exponent = _mm_sub_epi32(*exponent, f_bias);
    *exponent = _mm_sub_epi32(*exponent, exp_fix);

    data->fix = (data->fix & mantissa_mask) | reduce_mask;
}

// SIMD
__m128 log2_deg2_simd(__m128 x)
{
    union num_s data = {.flt = x};
    __m128i exponent;

    reduce_float_simd(&data, &exponent);

    // Pipelining
    __m128 y0, y;
    y0 = deg2_co3 + _mm_cvtepi32_ps(exponent);

    y = deg2_co1 * data.flt + deg2_co2;

    return y * data.flt + y0;
}

__m128 log2_deg4_simd(__m128 x)
{
    union num_s data = {.flt = x};
    __m128i exponent;

    reduce_float_simd(&data, &exponent);

    // Pipelining
    __m128 y0, x2, y, z;
    y0 = deg4_co5 + _mm_cvtepi32_ps(exponent);

    x2 = data.flt * data.flt;

    y = deg4_co2 * data.flt + deg4_co3;
    y = deg4_co1 * x2 + y;
    z = deg4_co4 * data.flt + y0;

    return y * x2 + z;
}

__m128 log2_artanh_simd(__m128 x)
{
    union num_s data = {.flt = x};
    __m128i exponent;

    reduce_float_simd(&data, &exponent);

    // Pipelining
    __m128 q, q2, y;

    q = (data.flt - f_one) / (data.flt + f_one);
    q2 = q * q;

    y = one_third + q2 * one_fifth;
    y = y * q2 + f_one;
    y = y * q * ln2_inverse_2;

    return y + _mm_cvtepi32_ps(exponent);
}

// With Lookup table
float log2_lookup_scalar(float x)
{
    union num data = {.flt = x};
    int exponent, index;

    reduce_float_scalar(&data, &exponent);

    index = (data.fix & 0x7FFFFF) >> (23 - LOG_LOOKUP_TABLE_SIZE);

    return log_lookup_table[index] + exponent;
}

__m128 log2_lookup_simd(__m128 x)
{
    union num_s data = {.flt = x};
    __m128i exponent, index;

    reduce_float_simd(&data, &exponent);

    index = _mm_srli_epi32(data.fix & mantissa_mask, (23 - LOG_LOOKUP_TABLE_SIZE));

    __m128 y = _mm_set_ps(
        log_lookup_table[((__v4si)index)[3]],
        log_lookup_table[((__v4si)index)[2]],
        log_lookup_table[((__v4si)index)[1]],
        log_lookup_table[((__v4si)index)[0]]);

    return y + _mm_cvtepi32_ps(exponent);
}

__m128 log2_glibc_simd(__m128 x)
{
    union num_s data = {.flt = x};

    __m128i tmp, i, top, k;
    __m128 invc, logc, r, y0, r2, y, p;
    union num_s z;

    tmp = data.fix - glibc_off;
    i = _mm_srli_epi32(tmp, (23 - 4));
    i &= glibc_mod16_mask;
    top = tmp & glibc_extract;
    z.fix = data.fix - top;
    k = _mm_srai_epi32(tmp, 23);

    invc = _mm_set_ps(
        glibc_inverse_c[((__v4si)i)[3]],
        glibc_inverse_c[((__v4si)i)[2]],
        glibc_inverse_c[((__v4si)i)[1]],
        glibc_inverse_c[((__v4si)i)[0]]);

    logc = _mm_set_ps(
        glibc_logc[((__v4si)i)[3]],
        glibc_logc[((__v4si)i)[2]],
        glibc_logc[((__v4si)i)[1]],
        glibc_logc[((__v4si)i)[0]]);

    r = z.flt * invc - f_one;
    y0 = logc + _mm_cvtepi32_ps(k);

    // Pipeline
    r2 = r * r;
    y = glibc_co1 * r + glibc_co2;
    y = glibc_co0 * r2 + y;
    p = glibc_co3 * r + y0;
    y = y * r2 + p;
    return y;
}
