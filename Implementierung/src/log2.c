#include "log2.h"

// Normal
float log2approx_deg2(float x){
    uint32_t ix, iz;
    int exponent;
    float m, y, a1 = -0.344845f, a2 = 2.024658f, a3 = 1.674873f;
    ix = *(uint32_t *)&x;
    exponent = ((int32_t) ix >> 23) - 127;
    iz = (ix & 0x7fffff) + 0x3f800000;
    m = *(float *) &iz;

    y = a1*m*m + a2*m + (exponent - a3);
    return y;
}

float log2approx_deg4(float x){
    uint32_t ix, iz;
    int exponent;
    float m, m2, y, a1 = -0.081615808f, 
	a2 = 0.64514236f, a3 = -2.1206751f, 
	a4 = 4.0700908f, a5 = -2.5128546f;

    ix = *(uint32_t *)&x;
    exponent = ((int32_t) ix >> 23) - 127;
    iz = (ix & 0x7fffff) + 0x3f800000;
    m = *(float *) &iz;

    m2 = m*m;
    y = a1*m2 + a2 * m;
    y = y*m2 + a3*m2 + a4*m + (a5 + exponent);
    return y;
}

float log2approx_arctanh(float x){
    uint32_t ix, iz;
    int exponent;
    float m, y, q, q2, ln2_2 = 2.88539008178f, third = 0.33333333333f;

    ix = *(uint32_t *)&x;
    exponent = ((int32_t) ix >> 23) - 127;
    iz = (ix & 0x7fffff) + 0x3f800000;
    m = *(float *) &iz;
    q = (m-1)/(m+1);
    q2 = q*q;
    y = (third + q2*0.2f)*q2 + 1;
    y = y*q*ln2_2 + exponent;
    return y;
}


// SIMD
__m128 log2deg2_sse(__m128 x){
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

__m128 log2deg4_sse(__m128 x){
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


__m128 log2arctanh_sse(__m128 x){
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
void init_table(unsigned n, float* table)
{
    int steps = (1 << n);
    float x = 1.0f + (1.0f / (float)( 1 <<(n + 1) ));
    float stepSize = 1.0f / (float)steps;
    for(int i = 0;  i < steps;  i++ )
    {
        table[i] = log2f(x);
        x += stepSize;
    }
}

float log2_lookup(unsigned n, float* table, float x){
    union num ix;
    ix.flt = x;
    int exponent, index;
    exponent = (ix.fix >> 23) - 127;
    index = (ix.fix & 0x7FFFFF) >> (23 - n);

    return table[index] + exponent;
}

__m128 log2_lookup_simd(unsigned n, float* table, __m128 x) {
    __m128i expi = _mm_srai_epi32((__m128i) x, 23);
    expi = _mm_sub_epi32(expi, _mm_set1_epi32(127));
    __m128 exponent = _mm_cvtepi32_ps(expi);

    __m128i m = _mm_set1_epi32(0x7fffff);
    m &= (__m128i) x;
    m = _mm_srai_epi32(m, (23 - n));
    __m128 y = _mm_set_ps(
            table[((__v4si) m)[0]],
            table[((__v4si) m)[1]],
            table[((__v4si) m)[2]],
            table[((__v4si) m)[3]]
    );
    y += exponent;
    return y;
}
