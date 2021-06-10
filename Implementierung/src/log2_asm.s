.intel_syntax noprefix
.global log2approx_deg2_asm
.global log2approx_deg4_asm
.global log2approx_arctanh_asm
.global log2_lookup_asm

// Make sure LOG_LOOKUP_TABLE_SIZE in log2.h is the same
.equ LOG_LOOKUP_TABLE_SIZE, 14


// !!! USING CONSTANTS FROM log2.c !!!

.text
// float log2approx_deg2_asm(float val);
log2approx_deg2_asm:

	// Extract exponent from IEEE Floating Number
	movd eax, xmm0
	shr eax, 23
	sub eax, 127

	// Reduce floating point number into
	// val = 2^k * z form
	// we have already found k value, so reduce val to z
	// Where 1 <= z < 2
	// So set exponent to 127 (with bias) where mantissa remains same
	movd ebx, xmm0
	or ebx, [reduce_mask]

	movd xmm0, ebx

	// Apply approximation
	// log_2(val) = -0.344845 * z^2 + 2.024658 * z - 1.674873 + exponent
	movss xmm15, xmm0
	mulss xmm15, xmm15 
	mulss xmm15, [rip + deg2_co1]

	mulss xmm0, [rip + deg2_co2]

	addss xmm0, xmm15
	addss xmm0, [rip + deg2_co3]
	
	cvtsi2ss xmm15, eax

	addss xmm0, xmm15
	
	ret


// float log2approx_deg4_asm(float val);
log2approx_deg4_asm:

	// Extract exponent from IEEE Floating Number
	movd eax, xmm0
	shr eax, 23
	sub eax, 127

	// Reduce floating point number into
	// val = 2^k * z form
	// we have already found k value, so reduce val to z
	// Where 1 <= z < 2
	// So set exponent to 127 (with bias) where mantissa remains same
	movd ebx, xmm0
	or ebx, [reduce_mask]

	movd xmm0, ebx

	// Apply approximation
	// log_2(val) = -0.081615808 * z^4 + 0.64514236 * z^3 + 0.64514236 * z^2 + 4.0700908 * z + -2.5128546 + exponent
	movss xmm15, xmm0 
	mulss xmm15, xmm15 

	movss xmm14, xmm15
	mulss xmm14, [rip + deg4_co1]

	movss xmm13, xmm0
	mulss xmm13, [rip + deg4_co2]

	addss xmm14, xmm13

	addss xmm14, [rip + deg4_co3]
	mulss xmm14, xmm15

	movss xmm12, xmm0
	mulss xmm12, [rip + deg4_co4]

	addss xmm14, xmm12
	addss xmm14, [rip + deg4_co5]

	cvtsi2ss xmm15, eax
	addss xmm14, xmm15

	movss xmm0, xmm14
	
	ret

// float log2approx_deg4_asm(float val);
log2approx_arctanh_asm:


	// Extract exponent from IEEE Floating Number
	movd eax, xmm0
	shr eax, 23
	sub eax, 127

	// Reduce floating point number into
	// val = 2^k * z form
	// we have already found k value, so reduce val to z
	// Where 1 <= z < 2
	// So set exponent to 127 (with bias) where mantissa remains same
	movd ebx, xmm0
	or ebx, [reduce_mask]

	movd xmm0, ebx

	// x = (z-1) / (z + 1)
	//log_2(val) = 2 * (x + x^3/3 + x^5/5) / ln2

	// q
	movss xmm15, xmm0
	subss xmm15, [rip + f_one]

	movss xmm14, xmm0
	addss xmm14, [rip + f_one]

	// xmm15 = q
	divss xmm15, xmm14

	// xmm14 = q^2
	movss xmm14, xmm15
	mulss xmm14, xmm14

	// y
	movss xmm0, xmm14
	mulss xmm0, [rip + one_fifth]
	addss xmm0, [rip + one_third]
	mulss xmm0, xmm14
	addss xmm0, [rip + f_one]

	mulss xmm0, xmm15
	mulss xmm0, [rip + ln2_inverse_2]

	cvtsi2ss xmm15, eax
	addss xmm0, xmm15 

	ret

// float log2_lookup(float x)
log2_lookup_asm:

	// Extract exponent from IEEE Floating Number
	movd eax, xmm0
	shr eax, 23
	sub eax, 127

	// Get first n bits of mantiss to look in table
	movd ebx, xmm0
	and ebx, [rip + mantissa_mask]
	shr ebx, (23 - LOG_LOOKUP_TABLE_SIZE)

	// log_lookup_table[index] + exponent
	cvtsi2ss xmm0, eax
	addss xmm0, [log_lookup_table + 4*ebx]

	ret

