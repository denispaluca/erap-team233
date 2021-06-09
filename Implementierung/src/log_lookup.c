//
// Created by denis on 09.06.21.
//
#include "log_lookup.h"
#include <math.h>

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

union data {float flt; int fix;};
float log2_lookup(unsigned n, float* table, float x){
    union data ix;
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
