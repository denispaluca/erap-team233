.intel_syntax noprefix
.global log2approx_deg2_simd_asm
.global log2approx_deg4_simd_asm
.global log2approx_arctanh_simd_asm
.global log2_lookup_simd_asm

// Make sure LOG_LOOKUP_TABLE_SIZE in log2.h is the same
.equ LOG_LOOKUP_TABLE_SIZE, 16

// !!! USING CONSTANTS FROM log2.c !!!

.text

// __m128 log2approx_deg2_sse_asm(__m128 x)
log2approx_deg2_simd_asm:
    
    // Extract exponents from IEEE Floating Numbers
    movaps xmm15, xmm0
    psrld xmm15, 23

    // If exponent zero
    pxor xmm14, xmm14
    pcmpeqd xmm14, xmm15

    // exp_fix
    movdqa xmm13, xmm14
    pand xmm13, [rip + normalize_exp] 

    pand xmm14, [rip + normalize_mask]
    pxor xmm14, [rip + f_one]

    mulps xmm0, xmm14

    // Recalculate exponents
    movaps xmm15, xmm0
    psrld xmm15, 23
    psubd xmm15, xmm13

    psubd xmm15, [rip + f_bias]

    // Reduce floating point numbers into
	// val = 2^k * z form
	// we have already found k values, so reduce vals to zs
	// Where 1 <= z < 2
	// So set exponents to 127 (with bias) where mantissa remains same
    pand xmm0, [rip + mantissa_mask]
    por xmm0, [rip + reduce_mask]

    // Apply approximation
	// log_2(val) = -0.344845 * z^2 + 2.024658 * z - 1.674873 + exponent

    cvtdq2ps xmm15, xmm15
    addps xmm15, [rip + deg2_co3]

	movaps xmm14, [rip + deg2_co1]
	mulps xmm14, xmm0
	addps xmm14, [rip + deg2_co2]

	mulps xmm0, xmm14
	addps xmm0, xmm15

    ret

// __m128 log2approx_deg4_simd_asm(__m128 x)
log2approx_deg4_simd_asm:

    // Extract exponents from IEEE Floating Numbers
    movaps xmm15, xmm0
    psrld xmm15, 23

    // If exponent zero
    pxor xmm14, xmm14
    pcmpeqd xmm14, xmm15

    // exp_fix
    movdqa xmm13, xmm14
    pand xmm13, [rip + normalize_exp] 

    pand xmm14, [rip + normalize_mask]
    pxor xmm14, [rip + f_one]

    mulps xmm0, xmm14

    // Recalculate exponents
    movaps xmm15, xmm0
    psrld xmm15, 23
    psubd xmm15, xmm13

    psubd xmm15, [rip + f_bias]

    // Reduce floating point numbers into
	// val = 2^k * z form
	// we have already found k values, so reduce vals to zs
	// Where 1 <= z < 2
	// So set exponents to 127 (with bias) where mantissa remains same
    pand xmm0, [rip + mantissa_mask]
    por xmm0, [rip + reduce_mask]

    
    // Apply approximation
	// log_2(val) = -0.081615808 * z^4 + 0.64514236 * z^3 + 0.64514236 * z^2 + 4.0700908 * z + -2.5128546 + exponent
	
	// y0
    cvtdq2ps xmm15, xmm15
	addps xmm15, [rip + deg4_co5]

	// x2
	movaps xmm14, xmm0
	mulps xmm14, xmm14

	// y
	movaps xmm13, [rip + deg4_co2]
	mulps xmm13, xmm0
	addps xmm13, [rip + deg4_co3]

	movaps xmm12, [rip + deg4_co1]
	mulps xmm12, xmm14

	addps xmm13, xmm12
	// y

	// z
	movaps xmm11, [rip + deg4_co4]
	mulps xmm11, xmm0
	addps xmm11, xmm15

	movaps xmm0, xmm13
	mulps xmm0, xmm14
	addps xmm0, xmm11

    ret

// __m128 log2approx_arctanh_simd_asm(__m128 x)
log2approx_arctanh_simd_asm:

    // Extract exponents from IEEE Floating Numbers
    movaps xmm15, xmm0
    psrld xmm15, 23

    // If exponent zero
    pxor xmm14, xmm14
    pcmpeqd xmm14, xmm15

    // exp_fix
    movdqa xmm13, xmm14
    pand xmm13, [rip + normalize_exp] 

    pand xmm14, [rip + normalize_mask]
    pxor xmm14, [rip + f_one]

    mulps xmm0, xmm14

    // Recalculate exponents
    movaps xmm15, xmm0
    psrld xmm15, 23
    psubd xmm15, xmm13

    psubd xmm15, [rip + f_bias]

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

    cvtdq2ps xmm15, xmm15
    addps xmm0, xmm15

    ret

log2_lookup_simd_asm:

    // Extract exponents from IEEE Floating Numbers
    movaps xmm15, xmm0
    psrld xmm15, 23

    // If exponent zero
    pxor xmm14, xmm14
    pcmpeqd xmm14, xmm15

    // exp_fix
    movdqa xmm13, xmm14
    pand xmm13, [rip + normalize_exp] 

    pand xmm14, [rip + normalize_mask]
    pxor xmm14, [rip + f_one]

    mulps xmm0, xmm14

    // Recalculate exponents
    movaps xmm15, xmm0
    psrld xmm15, 23
    psubd xmm15, xmm13

    psubd xmm15, [rip + f_bias]

    cvtdq2ps xmm15, xmm15

    // Get first n bits of mantiss to look in table
    pand xmm0, [rip + mantissa_mask]
    psrld xmm0, (23 - LOG_LOOKUP_TABLE_SIZE)
    
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
    movaps xmm14, xmm0
    pshufd xmm0, xmm14, 0b10010011

    por xmm0, xmm12
    movaps xmm14, xmm0
    pshufd xmm0, xmm14, 0b10010011

    por xmm0, xmm11
    movaps xmm14, xmm0
    pshufd xmm0, xmm14, 0b10010011

    por xmm0, xmm10
    movaps xmm14, xmm0

    pshufd xmm0,xmm0, 0b00011011

    addps xmm0, xmm15

    ret
