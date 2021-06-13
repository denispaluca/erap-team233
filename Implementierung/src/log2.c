#include "log2.h"

void reduce_float(union num* data, int* exponent){
    
    // Get exponent omitting sign bit
    *exponent = ((data->fix << 1) >> 24);

    // Special case for denormal floating numbers
    if (*exponent == 0) {
        data->flt *= normalize_const[0]; /* Normalizefloating number */
        *exponent = (data->fix >> 23) - 23; /* Recalculate exponent considering exponent used for normalization*/
    }
    
    // Subtracting bias
    *exponent -= f_bias[0];

    data->fix = (data->fix & mantissa_mask[0]) | reduce_mask[0];
}

// Normal
float log2approx_deg2(float x) {
    union num data = { .flt = x };
    int exponent;

    reduce_float(&data, &exponent);

    return deg2_co1[0] * data.flt * data.flt + deg2_co2[0] * data.flt + deg2_co3[0] + exponent;
}

float log2approx_deg4(float x) {
    union num data = { .flt = x };
    int exponent;

    reduce_float(&data, &exponent);

    float m2 = data.flt * data.flt;
    float y = deg4_co1[0] * m2 + deg4_co2[0] * data.flt;

    return y * m2 + deg4_co3[0] * m2 + deg4_co4[0] * data.flt + deg4_co5[0] + exponent;
}

float log2approx_arctanh(float x) {
    union num data = { .flt = x };
    int exponent;

    reduce_float(&data, &exponent);

    float q = (data.flt-1)/(data.flt+1);
    float q2 = q*q;
    float y = (one_third[0] + q2 * one_fifth[0]) * q2 + 1;

    return y * q * ln2_inverse_2[0] + exponent;
}

void reduce_float_simd(union num_s* data, __m128i* exponent){

    /* 
    1.0f:       0 01111111 00000000000000000000000
    2^23f:      0 10010110 00000000000000000000000
    xor mask:   0 11101001 00000000000000000000000
    zero mask   0 00000000 00000000000000000000000

    For exponent recalcultaion easily recalculate exponent for all and subtraact 23 for denormalized ones

    so that you can easily multiply by 2^23f
    */

    // Get exponent omitting sign bit
    *exponent = (data->fix << 1) >> 24;

    // Special case for denormal floating numbers
    union num_s mask;
    mask.fix = _mm_cmpeq_epi32(*exponent, _mm_setzero_si128());
    __m128i exp_fix = _mm_load_si128((const __m128i*) normalize_exp) & mask.fix;
    mask.fix &= _mm_load_si128((const __m128i*) normalize_mask);
    mask.fix ^= _mm_loadu_si128((const __m128i*) f_one);

    data->flt *= mask.flt;

    *exponent = data->fix >> 23;
    *exponent -= _mm_load_si128((const __m128i*) f_bias);
    *exponent -= exp_fix;

    data->fix = (data->fix & _mm_load_si128((const __m128i*) mantissa_mask)) | _mm_load_si128( (const __m128i*)reduce_mask);

}


// SIMD
__m128 log2approx_deg2_simd(__m128 x) {
    union num_s data = { .flt = x };
    __m128i exponent;

    reduce_float_simd(&data, &exponent);

    return _mm_load_ps(deg2_co1) * data.flt * data.flt + 
           _mm_load_ps(deg2_co2) * data.flt + _mm_load_ps(deg2_co3) + _mm_cvtepi32_ps(exponent);
}

__m128 log2approx_deg4_simd(__m128 x) {
    union num_s data = { .flt = x };
    __m128i exponent;

    reduce_float_simd(&data, &exponent);

    __m128 m2 = data.flt * data.flt;
    __m128 y = _mm_load_ps(deg4_co1) * m2 + _mm_load_ps(deg4_co2) * data.flt;

    return y * m2 + _mm_load_ps(deg4_co3) * m2 + _mm_load_ps(deg4_co4) * data.flt + _mm_load_ps(deg4_co5) + _mm_cvtepi32_ps(exponent);
}


__m128 log2approx_arctanh_simd(__m128 x) {
    union num_s data = { .flt = x };
    __m128i exponent;

    reduce_float_simd(&data, &exponent);

    __m128 q = (data.flt - _mm_load_ps(f_one)) / (data.flt + _mm_load_ps(f_one));
    __m128 q2 = q * q;
    __m128 y = (_mm_load_ps(one_third) + q2 * _mm_load_ps(one_fifth)) * q2 + _mm_load_ps(f_one);

    return y * q * _mm_load_ps(ln2_inverse_2) + _mm_cvtepi32_ps(exponent);
}


// With Lookup table

float log2_lookup(float x) {
    union num data = { .flt = x };
    int exponent, index;

    exponent = (data.fix >> 23) - f_bias[0];
    index = (data.fix & mantissa_mask[0]) >> (23 - LOG_LOOKUP_TABLE_SIZE);

    return log_lookup_table[index] + exponent;
}

__m128 log2_lookup_simd(__m128 x) {
    union num_s data = { .flt = x };
    __m128i exponent, index;

    exponent = (data.fix >> 23) - _mm_load_si128((const __m128i*) f_bias);

    index = (data.fix &  _mm_load_si128((const __m128i*) mantissa_mask)) >> (23 - LOG_LOOKUP_TABLE_SIZE);

    __m128 y = _mm_set_ps(
            log_lookup_table[((__v4si) index)[0]],
            log_lookup_table[((__v4si) index)[1]],
            log_lookup_table[((__v4si) index)[2]],
            log_lookup_table[((__v4si) index)[3]]
    );
    
    y += _mm_cvtepi32_ps(exponent);
    return y;
}
