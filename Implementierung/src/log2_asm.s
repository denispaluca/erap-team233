.intel_syntax noprefix
.global log2approx_deg2_asm
.global log2approx_deg4_asm
.global log2approx_arctanh_asm
.global log2_lookup_asm

// Make sure LOG_LOOKUP_TABLE_SIZE in log2.h is the same
.equ LOG_LOOKUP_TABLE_SIZE, 16


// !!! USING CONSTANTS FROM log2.c !!!

.text
// float log2approx_deg2_asm(float val);
.align 16
log2approx_deg2_asm:

	// Extract exponent from IEEE Floating Number
	movd eax, xmm0
	shr eax, 23

	// // Special case for denormal floating numbers
	cmp eax, 0
	jnz .Lnormal_deg2

	mulss xmm0, [rip + deg2_consts]
	movd eax, xmm0
	shr eax, 23
	sub eax, 23

	.Lnormal_deg2:
	sub eax, 127

	// Reduce floating point number into
	// val = 2^k * z form
	// we have already found k value, so reduce val to z
	// Where 1 <= z < 2
	// So set exponent to 127 (with bias) where mantissa remains same
	movd ecx,xmm0
	and ecx, 0x7FFFFF
	or ecx, 0x3F800000

	movd xmm0, ecx

	// Apply approximation
	// log_2(val) = -0.344845 * z^2 + 2.024658 * z - 1.674873 + exponent

	cvtsi2ss xmm15, eax
	addss xmm15, [rip + deg2_consts + 1*4]

	movss xmm14, [rip + deg2_consts + 2*4]
	mulss xmm14, xmm0
	addss xmm14, [rip + deg2_consts + 3*4]

	mulss xmm0, xmm14
	addss xmm0, xmm15
	
	ret


// float log2approx_deg4_asm(float val);
log2approx_deg4_asm:
	// Extract exponent from IEEE Floating Number
	movd eax, xmm0
	shr eax, 23

	// // Special case for denormal floating numbers
	cmp eax, 0
	jnz .Lnormal_deg4

	mulss xmm0, [rip + deg4_consts]
	movd eax, xmm0
	shr eax, 23
	sub eax, 23

	.Lnormal_deg4:
	sub eax, 127

	// Reduce floating point number into
	// val = 2^k * z form
	// we have already found k value, so reduce val to z
	// Where 1 <= z < 2
	// So set exponent to 127 (with bias) where mantissa remains same
	movd ecx, xmm0
	and ecx, 0x7FFFFF
	or ecx, 0x3F800000

	movd xmm0, ecx

	// Apply approximation
	// log_2(val) = -0.081615808 * z^4 + 0.64514236 * z^3 + 0.64514236 * z^2 + 4.0700908 * z + -2.5128546 + exponent
	
	// y0
	cvtsi2ss xmm15, eax
	addss xmm15, [rip + deg4_consts + 1*4]

	// x2
	movss xmm14, xmm0
	mulss xmm14, xmm14

	// y
	movss xmm13, [rip + deg4_consts + 2*4]
	mulss xmm13, xmm0
	addss xmm13, [rip + deg4_consts + 3*4]

	movss xmm12, [rip + deg4_consts + 4*4]
	mulss xmm12, xmm14

	addss xmm13, xmm12
	// y

	// z
	movss xmm11, [rip + deg4_consts + 5*4]
	mulss xmm11, xmm0
	addss xmm11, xmm15

	movss xmm0, xmm13
	mulss xmm0, xmm14
	addss xmm0, xmm11

	ret

// float log2approx_deg4_asm(float val);
log2approx_arctanh_asm:

	// Extract exponent from IEEE Floating Number
	movd eax, xmm0
	shr eax, 23

	// // Special case for denormal floating numbers
	cmp eax, 0
	jnz .Lnormal_arctanh

	mulss xmm0, [rip + artanh_consts]
	movd eax, xmm0
	shr eax, 23
	sub eax, 23

	.Lnormal_arctanh:
	sub eax, 127	

	// Reduce floating point number into
	// val = 2^k * z form
	// we have already found k value, so reduce val to z
	// Where 1 <= z < 2
	// So set exponent to 127 (with bias) where mantissa remains same
	movd ecx, xmm0
	and ecx, 0x7FFFFF
	or ecx, 0x3F800000

	movd xmm0, ecx

	// x = (z-1) / (z + 1)
	//log_2(val) = 2 * (x + x^3/3 + x^5/5) / ln2

	// xmm13 = 1.0
	movss xmm13, [rip + artanh_consts + 1*4]

	// q
	movss xmm15, xmm0
	subss xmm15, xmm13

	movss xmm14, xmm0
	addss xmm14, xmm13

	// xmm15 = q
	divss xmm15, xmm14

	// xmm14 = q^2
	movss xmm14, xmm15
	mulss xmm14, xmm14

	// y
	movss xmm0, xmm14
	mulss xmm0, [rip + artanh_consts + 2*4]
	addss xmm0, [rip + artanh_consts + 3*4]
	mulss xmm0, xmm14
	addss xmm0, xmm13

	mulss xmm0, xmm15
	mulss xmm0, [rip + artanh_consts + 4*4]

	cvtsi2ss xmm15, eax
	addss xmm0, xmm15 

	ret

// float log2_lookup(float x)
log2_lookup_asm:

	// Extract exponent from IEEE Floating Number
	movd eax, xmm0
	shr eax, 23

	// // Special case for denormal floating numbers
	cmp eax, 0
	jnz .Lnormal_lookup

	mulss xmm0, [rip + normalize_const]
	movd eax, xmm0
	shr eax, 23
	sub eax, 23

	.Lnormal_lookup:
	sub eax, 127

	// Get first n bits of mantiss to look in table
	movd ecx, xmm0
	and ecx, 0x7FFFFF
	shr ecx, (23 - LOG_LOOKUP_TABLE_SIZE)

	// log_lookup_table[index] + exponent
	cvtsi2ss xmm0, eax
	addss xmm0, [log_lookup_table + 4*ecx]

	ret
