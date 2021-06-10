.intel_syntax noprefix
.global log2approx_deg2_simd_asm
.global log2approx_deg4_simd_asm
.global log2approx_arctanh_simd_asm
.global log2_lookup_simd_asm

// Make sure LOG_LOOKUP_TABLE_SIZE in log2.h is the same
.equ LOG_LOOKUP_TABLE_SIZE, 14

// !!! USING CONSTANTS FROM log2.c !!!

.text

// __m128 log2approx_deg2_sse_asm(__m128 x)
log2approx_deg2_simd_asm:
    
    // Extract exponents from IEEE Floating Numbers
    movaps xmm15, xmm0
    psrld xmm15, 23
    psubd xmm15, [rip + f_bias]
    cvtdq2ps xmm15, xmm15

    // Reduce floating point numbers into
	// val = 2^k * z form
	// we have already found k values, so reduce vals to zs
	// Where 1 <= z < 2
	// So set exponents to 127 (with bias) where mantissa remains same
    pand xmm0, [rip + mantissa_mask]
    por xmm0, [rip + reduce_mask]

    movaps xmm14, xmm0
    mulps xmm14, xmm14
    mulps xmm14, [rip + deg2_co1]

    mulps xmm0, [rip + deg2_co2]

    addps xmm0, xmm14
    addps xmm0, [rip + deg2_co3]
    addps xmm0, xmm15

    ret

// __m128 log2approx_deg4_simd_asm(__m128 x)
log2approx_deg4_simd_asm:

    // Extract exponents from IEEE Floating Numbers
    movaps xmm15, xmm0
    psrld xmm15, 23
    psubd xmm15, [rip + f_bias]
    cvtdq2ps xmm15, xmm15

    // Reduce floating point numbers into
	// val = 2^k * z form
	// we have already found k values, so reduce vals to zs
	// Where 1 <= z < 2
	// So set exponents to 127 (with bias) where mantissa remains same
    pand xmm0, [rip + mantissa_mask]
    por xmm0, [rip + reduce_mask]

    movaps xmm14, xmm0
    mulps xmm14, xmm14

    movaps xmm13, xmm14
    mulps xmm13, [rip + deg4_co1]

    movaps xmm12, xmm0
    mulps xmm12, [rip + deg4_co2]

    addps xmm13, xmm12

    addps xmm13, [rip + deg4_co3]
    mulps xmm13, xmm14

    movaps xmm11, xmm0
    mulps xmm11, [rip + deg4_co4]

    addps xmm13, xmm11
    addps xmm13, [rip + deg4_co5]
    addps xmm13, xmm15

    movaps xmm0, xmm13

    ret

// __m128 log2approx_arctanh_simd_asm(__m128 x)
log2approx_arctanh_simd_asm:

    // Extract exponents from IEEE Floating Numbers
    movaps xmm15, xmm0
    psrld xmm15, 23
    psubd xmm15, [rip + f_bias]
    cvtdq2ps xmm15, xmm15

    // Reduce floating point numbers into
	// val = 2^k * z form
	// we have already found k values, so reduce vals to zs
	// Where 1 <= z < 2
	// So set exponents to 127 (with bias) where mantissa remains same
    pand xmm0, [rip + mantissa_mask]
    por xmm0, [rip + reduce_mask]

    movaps xmm14, xmm0
    subps xmm14, [rip + f_one]

    movaps xmm13, xmm0
    addps xmm13, [rip + f_one]

    divps xmm14, xmm13

    movaps xmm13, xmm14
    mulps xmm13, xmm13

    movaps xmm0, xmm13
    mulps xmm0, [rip + one_fifth]
    addps xmm0, [rip + one_third]
    mulps xmm0, xmm13
    addps xmm0, [rip + f_one]

    mulps xmm0, xmm14
    mulps xmm0, [rip + ln2_inverse_2]
    addps xmm0, xmm15

    ret

log2_lookup_simd_asm:

    // Extract exponents from IEEE Floating Numbers
    movaps xmm15, xmm0
    psrld xmm15, 23
    psubd xmm15, [rip + f_bias]
    cvtdq2ps xmm15, xmm15

    // Get first n bits of mantiss to look in table
    pand xmm0, [rip + mantissa_mask]
    psrld xmm0, (23 - LOG_LOOKUP_TABLE_SIZE)


    // WARNING: this is probably the worst possible solution.
    // will work on that later
    movups xmm14, xmm0

    movd eax, xmm0
    movd xmm13, [log_lookup_table + 4*eax]
    pshufd xmm0, xmm14, 0x01
    
    movd eax, xmm0
    movd xmm12, [log_lookup_table + 4*eax]
    pshufd xmm0, xmm14, 0x02
    
    movd eax, xmm0
    movd xmm11, [log_lookup_table + 4*eax]
    pshufd xmm0, xmm14, 0x03

    movd eax, xmm0
    movd xmm10, [log_lookup_table + 4*eax]

    pxor xmm0, xmm0

    por xmm0, xmm13
    movups xmm14, xmm0
    pshufd xmm0, xmm14, 0b10010011

    por xmm0, xmm12
    movups xmm14, xmm0
    pshufd xmm0, xmm14, 0b10010011

    por xmm0, xmm11
    movups xmm14, xmm0
    pshufd xmm0, xmm14, 0b10010011

    por xmm0, xmm10
    movups xmm14, xmm0

    addps xmm0, xmm15 

    ret
