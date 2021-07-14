.intel_syntax noprefix
.global log2_deg2_simd_asm
.global log2_deg4_simd_asm
.global log2_artanh_simd_asm
.global log2_lookup_simd_asm

// !!! USING CONSTANTS FROM log2_consts.c !!!

// Make sure LOG_LOOKUP_TABLE_SIZE in log2.h is the same
.equ LOG_LOOKUP_TABLE_SIZE, 16
.equ MANTISSA_BITS, 23

// Macro for reduce_float_simd() inline function
.macro REDUCE_FLOAT_SIMD reduce = 1
    // Extract exponents from IEEE Floating Numbers
    movaps xmm15, xmm0
    psrld xmm15, MANTISSA_BITS

    // If exponent zero
    pxor xmm14, xmm14
    pcmpeqd xmm14, xmm15

    // exp_fix
    movdqa xmm13, xmm14
    pand xmm13, [rip + normalize_exp] 

    pand xmm14, [rip + normalize_mask]
    pxor xmm14, [rip + one_packed]

    mulps xmm0, xmm14

    // Recalculate exponents
    movaps xmm15, xmm0
    psrld xmm15, MANTISSA_BITS
    psubd xmm15, xmm13

    psubd xmm15, [rip + bias_packed]

    .if \reduce
    // Reduce floating point numbers into
	// val = 2^k * z form
	// we have already found k values, so reduce vals to zs
	// Where 1 <= z < 2
	// So set exponents to 127 (with bias) where mantissa remains same
    pand xmm0, [rip + mantissa_mask]
    por xmm0, [rip + reduce_mask]
    .endif
.endm

.text

// __m128 log2approx_deg2_sse_asm(__m128 x)
.align 16
log2_deg2_simd_asm:
    
    // Inline function to extract mantissa
    REDUCE_FLOAT_SIMD

    // Apply approximation
	// log_2(val) = -0.344845 * z^2 + 2.024658 * z - 1.674873 + exponent

    /*
	xmm15 = deg2_co3 + exponent
	xmm14 = deg2_co1 * z + deg2_co2
	xmm0  = xmm14 * z + xmm15
	*/

	// cast integer exponent to float
    cvtdq2ps xmm15, xmm15
    addps xmm15, [rip + deg2_co3]

	movaps xmm14, [rip + deg2_co1]
	mulps xmm14, xmm0
	addps xmm14, [rip + deg2_co2]

	// xmm0 = z * (deg2_co1 * z + deg2_co2) + deg2_co3 + exponent
	mulps xmm0, xmm14
	addps xmm0, xmm15

    ret

// __m128 log2_deg4_simd_asm(__m128 x)
.align 16
log2_deg4_simd_asm:

	// Inline function to extract mantissa
    REDUCE_FLOAT_SIMD

    // Apply approximation
	// log_2(z) = -0.081615808 * z^4 + 0.645146 * z^3 + 0.645146 * z^2 + 4.0700908 * z + -2.5128546 + exponent
	
	/*
	xmm15 = deg4_co5 + exponent
	xmm14 = z^2
	xmm13 = deg4_co2 * z + deg4_co3
	xmm12 = deg4_co1 * xmm14
	xmm13 = xmm13 + xmm12
	xmm11 = deg4_co4 * z + xmm15
	xmm0  = xmm12 * xmm14 + xmm15
	*/

    // cast integer exponent to float
    cvtdq2ps xmm15, xmm15
	addps xmm15, [rip + deg4_co5]

	// x^2
	movaps xmm14, xmm0
	mulps xmm14, xmm14

	// ===================================
	// xmm13 = deg4_co1 * z^2 + deg4_co2 * z + deg4_co3
	movaps xmm13, [rip + deg4_co2]
	mulps xmm13, xmm0
	addps xmm13, [rip + deg4_co3]

	movaps xmm12, [rip + deg4_co1]
	mulps xmm12, xmm14

	addps xmm13, xmm12
	// ===================================

	// ===================================
	// xmm11 = deg4_co4 * z + deg4_co5 + exponent
	movaps xmm11, [rip + deg4_co4]
	mulps xmm11, xmm0
	addps xmm11, xmm15
    // ===================================

	// xmm0 = z^2 * (deg4_co1 * z^2 + deg4_co2 * z + deg4_co3) + deg4_co4 * z + deg4_co5 + exponent
	movaps xmm0, xmm13
	mulps xmm0, xmm14
	addps xmm0, xmm11

    ret

// __m128 log2_artanh_simd_asm(__m128 x)
.align 16
log2_artanh_simd_asm:

    // Inline function to extract mantissa
    REDUCE_FLOAT_SIMD

    // Apply Approximation
	// log_2(z) = 2 * (z + 1/3 * z^3 + 1/5 * z^5) / ln2

    // q = (z-1) / (z + 1)
	//log_2(z) = 2 * (q + q^3/3 + q^5/5) / ln2

	/*
    xmm15 = exponent
	xmm14 = (z-1) / (z + 1) = q
	xmm13 = q^2
	xmm12 = 1.0
	xmm0  = (1/5 * q^5 + 1/3 * q^3 + q) * 2/ln2 + exponent
	*/

	// =====================================
	// xmm12 = 1.0
	// xmm14 = (z-1) / (z + 1)
    movaps xmm12, [rip + one_packed]

    movaps xmm14, xmm0
    subps xmm14, xmm12

    movaps xmm13, xmm0
    addps xmm13, xmm12

    divps xmm14, xmm13
    // =====================================

    // xmm13 = q^2
    movaps xmm13, xmm14
    mulps xmm13, xmm13

    // ====================================
	// xmm0 = 1/3 + 1/5 * q^2
    movaps xmm0, xmm13
    mulps xmm0, [rip + one_fifth]
    addps xmm0, [rip + one_third]
    // ====================================

    // ====================================
	// xmm0 = q^2 * (1/3 + 1/5 * q^2) + 1
    mulps xmm0, xmm13
    addps xmm0, xmm12
    // ====================================

    // ====================================
	// xmm0 = q * 2/ln2 * (q^2 * (1/3 + 1/5 * q^2) + 1)
    mulps xmm0, xmm14
    mulps xmm0, [rip + ln2_inverse_2]
    // ====================================

    // cast exponent to float
    cvtdq2ps xmm15, xmm15

    // add exponent to log_2(z) so log_2(val) = log_2(z) + exponent
    addps xmm0, xmm15

    ret
//__m128 log2_lookup_simd_asm(__m128 x)
.align 16
log2_lookup_simd_asm:

    // Inline function to extract mantissa
	// Lookup function loads denormalize const from deg2

	// Parameter 0 indicates that no reducing needed after extracting 
	// the exponent as we will take first n bits of mantissa as index
    REDUCE_FLOAT_SIMD 0

    cvtdq2ps xmm15, xmm15

    // Get first n bits of mantiss to look in table
    pand xmm0, [rip + mantissa_mask]
    psrld xmm0, (MANTISSA_BITS - LOG_LOOKUP_TABLE_SIZE)
    
    // xmm0 = n4 | n3 | n2 | n1

    // r8 = n2 | n1
    movq r8, xmm0

    // xmm0 = x | x | n4 | n3
    movhlps xmm0, xmm0

    // r9 = n4 | n3
    movq r9, xmm0

    // xmm14 = table[r9[31:0]]
    // xmm13 = table[r9[63:32]]
    movss xmm14, [log_lookup_table + 4*r9d]
    shr r9, 32
    movss xmm13, [log_lookup_table + 4*r9d]

    // xmm14 = x | x | table[n4] | table[n3]
    unpcklps xmm14, xmm13

    // xmm0 = table[r9[31:0]]
    // xmm13 = table[r9[63:32]]
    movss xmm0, [log_lookup_table + 4*r8d]
    shr r8, 32
    movss xmm13, [log_lookup_table + 4*r8d]

    // xmm0 = x | x | table[n2] | table[n1]
    unpcklps xmm0, xmm13
    
    // xmm0 = table[n4] | table[n3] | table[n2] | table[n1]
    movlhps xmm0, xmm14

    // Adding exponent
    addps xmm0, xmm15

    ret
