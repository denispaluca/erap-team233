#include "log2.h"

// Normal
float log2approx_deg2(float x){
    union num data;
    data.flt = x;

    int exponent = (data.fix >> 23) - 127;
    data.fix = (data.fix & mantissa_mask[0]) | reduce_mask[0];

    return deg2_co1[0] * data.flt * data.flt + deg2_co2[0] * data.flt + deg2_co3[0] + exponent;
}

float log2approx_deg4(float x){
    union num data;
    data.flt = x;

    int exponent = (data.fix >> 23) - 127;
    data.fix = (data.fix & mantissa_mask[0]) | reduce_mask[0];

    float m2 = data.flt * data.flt;
    float y = deg4_co1[0] * m2 + deg4_co2[0] * data.flt;

    return y * m2 + deg4_co3[0] * m2 + deg4_co4[0] * data.flt + deg4_co5[0] + exponent;
}

float log2approx_arctanh(float x){
    union num data;
    data.flt = x;

    int exponent = (data.fix >> 23) - 127;
    data.fix = (data.fix & mantissa_mask[0]) | reduce_mask[0];

    float q = (data.flt-1)/(data.flt+1);
    float q2 = q*q;
    float y = (one_third[0] + q2 * one_fifth[0]) * q2 + 1;

    return y * q * ln2_inverse_2[0] + exponent;
}


// SIMD
__m128 log2approx_deg2_simd(__m128 x){
    __m128i expi = _mm_set1_epi32(0x7f800000);
    expi &= (__m128i) x;
    expi >>= 23;
    expi = _mm_sub_epi32(expi, _mm_set1_epi32(127));
    __m128 exponent = _mm_cvtepi32_ps(expi);

    __m128 m = (__m128) _mm_set1_epi32(0x7fffff);
    m = _mm_and_ps(m, x);
    m = (__m128) _mm_add_epi32((__m128i) m, _mm_set1_epi32(0x3f800000));

    __m128 y = _mm_set1_ps(-0.344845f)*m + _mm_set1_ps(2.024658f);
    y = y*m + (exponent - _mm_set1_ps(1.674873f));
    return y;
}

__m128 log2approx_deg4_simd(__m128 x){
    __m128i expi = _mm_set1_epi32(0x7f800000);
    expi &= (__m128i) x;
    expi >>= 23;
    expi = _mm_sub_epi32(expi, _mm_set1_epi32(127));
    __m128 y, m, m2, exponent = _mm_cvtepi32_ps(expi);

    m = (__m128) _mm_set1_epi32(0x7fffff);
    m = _mm_and_ps(m, x);
    m = (__m128) _mm_add_epi32((__m128i) m, _mm_set1_epi32(0x3f800000));

    m2 = m*m;
    y = _mm_set1_ps(-0.081615808f)*m2 + _mm_set1_ps(0.64514236f)*m;
    y = (y + _mm_set1_ps(-2.1206751f))*m;
    y = (y + _mm_set1_ps(4.0700908f))*m + (exponent - _mm_set1_ps(2.5128546f));
    return y;
}


__m128 log2approx_arctanh_simd(__m128 x){
    __m128i expi = _mm_set1_epi32(0x7f800000);
    expi &= (__m128i) x;
    expi >>= 23;
    expi = _mm_sub_epi32(expi, _mm_set1_epi32(127));
    __m128 y, m, q, q2, xmm1, exponent = _mm_cvtepi32_ps(expi);

    m = (__m128) _mm_set1_epi32(0x7fffff);
    m = _mm_and_ps(m, x);
    m = (__m128) _mm_add_epi32((__m128i) m, _mm_set1_epi32(0x3f800000));

    xmm1 = _mm_set1_ps(1.0f);
    q = (m - xmm1)/(m + xmm1);

    q2 = q*q;
    y = (_mm_set1_ps(0.33333333333f) + q2*0.2f)*q2 + 1;
    y = y*q*2.88539008178f + exponent;

    return y;
}


// With Lookup table

float log2_lookup(float x) {
    union num ix;
    ix.flt = x;
    int exponent, index;
    exponent = (ix.fix >> 23) - 127;
    index = (ix.fix & 0x7FFFFF) >> (23 - LOG_LOOKUP_TABLE_SIZE);

    return log_lookup_table[index] + exponent;
}

__m128 log2_lookup_simd(__m128 x) {
    __m128i expi = _mm_srli_epi32((__m128i) x, 23);
    expi = _mm_sub_epi32(expi, _mm_set1_epi32(127));
    __m128 exponent = _mm_cvtepi32_ps(expi);

    __m128i m = _mm_set1_epi32(0x7fffff);
    m &= (__m128i) x;
    m = _mm_srli_epi32(m, (23 - LOG_LOOKUP_TABLE_SIZE));

    __m128 y = _mm_set_ps(
            log_lookup_table[((__v4si) m)[0]],
            log_lookup_table[((__v4si) m)[1]],
            log_lookup_table[((__v4si) m)[2]],
            log_lookup_table[((__v4si) m)[3]]
    );
    y += exponent;
    return y;
}
