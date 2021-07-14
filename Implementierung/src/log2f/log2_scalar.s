.intel_syntax noprefix
.global log2_deg2_scalar_asm
.global log2_deg4_scalar_asm
.global log2_artanh_scalar_asm
.global log2_lookup_scalar_asm

// !!! USING CONSTANTS FROM log2_consts.c !!!

// Make sure LOG_LOOKUP_TABLE_SIZE in log2.h is the same
.equ LOG_LOOKUP_TABLE_SIZE, 16
.equ EXPONENT_MASK, 0x3F800000
.equ MANTISSA_MASK, 0x7FFFFF
.equ BIAS, 127
.equ MANTISSA_BITS, 23

// Macro for reduce_float() inline function
.macro REDUCE_FLOAT label, const = "deg2", reduce = 1

	// Extract exponent from IEEE Floating Number
	movd eax, xmm0
	shr eax, MANTISSA_BITS

	// // Special case for denormal floating numbers
	cmp eax, 0
	jnz .Lnormal_\label

	mulss xmm0, [rip + \const\()_consts]
	movd eax, xmm0
	shr eax, MANTISSA_BITS
	sub eax, MANTISSA_BITS

	.Lnormal_\label:
	sub eax, BIAS	

	.if \reduce
	// Reduce floating point number into
	// val = 2^k * z form
	// we have already found k value, so reduce val to z
	// Where 1 <= z < 2
	// So set exponent to 127 (with bias) where mantissa remains same
	movd ecx, xmm0
	and ecx, MANTISSA_MASK
	or ecx, EXPONENT_MASK

	movd xmm0, ecx
	.endif
.endm

.text
// float log2_deg2_scalar_asm(float val);
.align 16
log2_deg2_scalar_asm:

	// Inline function to extract mantissa
	REDUCE_FLOAT "deg2", "deg2"

	// Apply approximation
	// log_2(z) = -0.344845 * z^2 + 2.024658 * z - 1.674873 + exponent

	/*
	xmm15 = deg2_co3 + exponent
	xmm14 = deg2_co1 * z + deg2_co2
	xmm0  = xmm14 * z + xmm15
	*/

	// cast integer exponent to float
	cvtsi2ss xmm15, eax
	addss xmm15, [rip + deg2_consts + 1*4]

	movss xmm14, [rip + deg2_consts + 2*4]
	mulss xmm14, xmm0
	addss xmm14, [rip + deg2_consts + 3*4]

	// xmm0 = z * (deg2_co1 * z + deg2_co2) + deg2_co3 + exponent
	mulss xmm0, xmm14
	addss xmm0, xmm15
	
	ret


// float log2_deg4_scalar_asm(float val);
.align 16
log2_deg4_scalar_asm:

	// Inline function to extract mantissa
	REDUCE_FLOAT "deg4", "deg4"

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
	cvtsi2ss xmm15, eax
	addss xmm15, [rip + deg4_consts + 1*4]

	// x^2
	movss xmm14, xmm0
	mulss xmm14, xmm14

	// ===================================
	// xmm13 = deg4_co1 * z^2 + deg4_co2 * z + deg4_co3
	movss xmm13, [rip + deg4_consts + 2*4]
	mulss xmm13, xmm0
	addss xmm13, [rip + deg4_consts + 3*4]

	movss xmm12, [rip + deg4_consts + 4*4]
	mulss xmm12, xmm14

	addss xmm13, xmm12
	// ===================================

	// ===================================
	// xmm11 = deg4_co4 * z + deg4_co5 + exponent
	movss xmm11, [rip + deg4_consts + 5*4]
	mulss xmm11, xmm0
	addss xmm11, xmm15
	// ===================================

	// xmm0 = z^2 * (deg4_co1 * z^2 + deg4_co2 * z + deg4_co3) + deg4_co4 * z + deg4_co5 + exponent
	movss xmm0, xmm13
	mulss xmm0, xmm14
	addss xmm0, xmm11

	ret

// float log2_deg4_scalar_asm(float val);
.align 16
log2_artanh_scalar_asm:

	// Inline function to extract mantissa
	REDUCE_FLOAT "artanh", "artanh"

	// Apply Approximation
	// log_2(z) = 2 * (z + 1/3 * z^3 + 1/5 * z^5) / ln2

	// q = (z-1) / (z + 1)
	//log_2(z) = 2 * (q + q^3/3 + q^5/5) / ln2

	/*
	xmm15 = (z-1) / (z + 1) = q
	xmm14 = q^2
	xmm13 = 1.0
	xmm0  = (1/5 * q^5 + 1/3 * q^3 + q) * 2/ln2 + exponent
	*/

	// =====================================
	// xmm13 = 1.0
	// xmm15 = (z-1) / (z + 1)
	movss xmm13, [rip + artanh_consts + 1*4]

	movss xmm15, xmm0
	subss xmm15, xmm13

	movss xmm14, xmm0
	addss xmm14, xmm13

	divss xmm15, xmm14
	// =====================================

	// xmm14 = q^2
	movss xmm14, xmm15
	mulss xmm14, xmm14

	// ====================================
	// xmm0 = 1/3 + 1/5 * q^2
	movss xmm0, xmm14
	mulss xmm0, [rip + artanh_consts + 2*4]
	addss xmm0, [rip + artanh_consts + 3*4]
	// ====================================

	// ====================================
	// xmm0 = q^2 * (1/3 + 1/5 * q^2) + 1
	mulss xmm0, xmm14
	addss xmm0, xmm13
	// ====================================

	// ====================================
	// xmm0 = q * 2/ln2 * (q^2 * (1/3 + 1/5 * q^2) + 1)
	mulss xmm0, xmm15
	mulss xmm0, [rip + artanh_consts + 4*4]
	// ====================================

	// cast exponent to float
	cvtsi2ss xmm15, eax

	// add exponent to log_2(z) so log_2(val) = log_2(z) + exponent
	addss xmm0, xmm15 
	ret

// float log2_lookup_scalar(float x)
.align 16
log2_lookup_scalar_asm:

	// Inline function to extract mantissa
	// Lookup function loads denormalize const from deg2

	// Parameter 0 indicates that no reducing needed after extracting 
	// the exponent as we will take first n bits of mantissa as index
	REDUCE_FLOAT "lookup", reduce = 0

	// Get first n bits of mantiss to look in table
	movd ecx, xmm0
	and ecx, MANTISSA_MASK
	shr ecx, (MANTISSA_BITS - LOG_LOOKUP_TABLE_SIZE)

	// log_lookup_table[index] + exponent

	// cast exponent to float
	cvtsi2ss xmm0, eax
	addss xmm0, [log_lookup_table + 4*ecx]

	ret
