#include "log2.h"

static void reduce_float_simd(union num_s *data, __m128i *exponent) {

    /* 
    1.0f:       0 01111111 00000000000000000000000
    2^23f:      0 10010110 00000000000000000000000
    xor mask:   0 11101001 00000000000000000000000
    zero mask   0 00000000 00000000000000000000000

    For exponent recalculation easily recalculate exponent for all and subtract 23 for denormalized ones

    so that you can easily multiply by 2^23f
    */

    // Get exponent (omitting sign bit)
    *exponent = _mm_srli_epi32(data->fix, 23);

    // Special case for denormal floating numbers
    union num_s mask;
    mask.fix = _mm_cmpeq_epi32(*exponent, _mm_setzero_si128());
    __m128i exp_fix = normalize_exp & mask.fix;
    mask.fix &= normalize_mask;
    mask.fix ^= (const __m128i) one_packed;

    data->flt *= mask.flt;

    *exponent = _mm_srli_epi32(data->fix, 23);
    *exponent = _mm_sub_epi32(*exponent, bias_packed);
    *exponent = _mm_sub_epi32(*exponent, exp_fix);

    data->fix = (data->fix & mantissa_mask) | reduce_mask;
}

// SIMD
__m128 log2_deg2_simd(__m128 x) {
    union num_s data = {.flt = x};
    __m128i exponent;

    reduce_float_simd(&data, &exponent);

    // Pipelining
    __m128 y0, y;
    y0 = deg2_co3 + _mm_cvtepi32_ps(exponent);

    y = deg2_co1 * data.flt + deg2_co2;

    return y * data.flt + y0;
}

__m128 log2_deg4_simd(__m128 x) {
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

__m128 log2_artanh_simd(__m128 x) {
    union num_s data = {.flt = x};
    __m128i exponent;

    reduce_float_simd(&data, &exponent);

    // Pipelining
    __m128 q, q2, y;

    q = (data.flt - one_packed) / (data.flt + one_packed);
    q2 = q * q;

    y = one_third + q2 * one_fifth;
    y = y * q2 + one_packed;
    y = y * q * ln2_inverse_2;

    return y + _mm_cvtepi32_ps(exponent);
}


__m128 log2_lookup_simd(__m128 x) {
    union num_s data = {.flt = x};
    __m128i exponent, index;

    reduce_float_simd(&data, &exponent);

    index = _mm_srli_epi32(data.fix & mantissa_mask, (23 - LOG_LOOKUP_TABLE_SIZE));

    __m128 y = _mm_set_ps(
            log_lookup_table[((__v4si) index)[3]],
            log_lookup_table[((__v4si) index)[2]],
            log_lookup_table[((__v4si) index)[1]],
            log_lookup_table[((__v4si) index)[0]]);

    return y + _mm_cvtepi32_ps(exponent);
}

/*
Source: https://code.woboq.org/userspace/glibc/sysdeps/ieee754/flt-32/e_log2f.c.html
*/
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
            glibc_inverse_c[((__v4si) i)[3]],
            glibc_inverse_c[((__v4si) i)[2]],
            glibc_inverse_c[((__v4si) i)[1]],
            glibc_inverse_c[((__v4si) i)[0]]);

    logc = _mm_set_ps(
            glibc_logc[((__v4si) i)[3]],
            glibc_logc[((__v4si) i)[2]],
            glibc_logc[((__v4si) i)[1]],
            glibc_logc[((__v4si) i)[0]]);

    r = z.flt * invc - one_packed;
    y0 = logc + _mm_cvtepi32_ps(k);

    // Pipeline
    r2 = r * r;
    y = glibc_co1 * r + glibc_co2;
    y = glibc_co0 * r2 + y;
    p = glibc_co3 * r + y0;
    y = y * r2 + p;
    return y;
}
