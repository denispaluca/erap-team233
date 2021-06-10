.intel_syntax noprefix
.global log2approx_deg2_asm
.global log2approx_deg4_asm
.global log2approx_arctanh_asm

.text
// float log2approx_deg2_asm(float val);
log2approx_deg2_asm:

	// Extract exponent from IEEE Floating Number
	movq rax, xmm0
	shr rax, 23
	sub rax, 127

	// Reduce floating point number into
	// val = 2^k * z form
	// we have already found k value, so reduce val to z
	// Where 1 <= z < 2
	// So set exponent to 127 (with bias) where mantissa remains same
	movq rbx, xmm0
	or rbx, [.reduce_mask]

	movq xmm0, rbx

	// Apply approximation
	// log_2(val) = -0.344845 * z^2 + 2.024658 * z - 1.674873 + exponent
	movss xmm15, xmm0
	mulss xmm15, xmm15 
	mulss xmm15, [rip + .deg2_co1]

	mulss xmm0, [rip + .deg2_co2]

	addss xmm0, xmm15
	addss xmm0, [rip + .deg2_co3]
	
	cvtsi2ss xmm15, rax

	addss xmm0, xmm15
	
	ret


// float log2approx_deg4_asm(float val);
log2approx_deg4_asm:

	// Extract exponent from IEEE Floating Number
	movq rax, xmm0
	shr rax, 23
	sub rax, 127

	// Reduce floating point number into
	// val = 2^k * z form
	// we have already found k value, so reduce val to z
	// Where 1 <= z < 2
	// So set exponent to 127 (with bias) where mantissa remains same
	movq rbx, xmm0
	or rbx, [.reduce_mask]

	movq xmm0, rbx

	// Apply approximation
	// log_2(val) = -0.081615808 * z^4 + 0.64514236 * z^3 + 0.64514236 * z^2 + 4.0700908 * z + -2.5128546 + exponent
	movss xmm15, xmm0 
	mulss xmm15, xmm15 

	movss xmm14, xmm15
	mulss xmm14, [rip + .deg4_co1]

	movss xmm13, xmm0
	mulss xmm13, [rip + .deg4_co2]

	addss xmm14, xmm13

	addss xmm14, [rip + .deg4_co3]
	mulss xmm14, xmm15

	movss xmm12, xmm0
	mulss xmm12, [rip + .deg4_co4]

	addss xmm14, xmm12
	addss xmm14, [rip + .deg4_co5]

	cvtsi2ss xmm15, rax
	addss xmm14, xmm15

	movss xmm0, xmm14
	
	ret

// float log2approx_deg4_asm(float val);
log2approx_arctanh_asm:


	// Extract exponent from IEEE Floating Number
	movq rax, xmm0
	shr rax, 23
	sub rax, 127

	// Reduce floating point number into
	// val = 2^k * z form
	// we have already found k value, so reduce val to z
	// Where 1 <= z < 2
	// So set exponent to 127 (with bias) where mantissa remains same
	movq rbx, xmm0
	or rbx, [.reduce_mask]

	movq xmm0, rbx

	// x = (z-1) / (z + 1)
	//log_2(val) = 2 * (x + x^3/3 + x^5/5) / ln2

	// q
	movss xmm15, xmm0
	subss xmm15, [rip + .f_one]

	movss xmm14, xmm0
	addss xmm14, [rip + .f_one]

	// xmm15 = q
	divss xmm15, xmm14

	// xmm14 = q^2
	movss xmm14, xmm15
	mulss xmm14, xmm14

	// y
	movss xmm0, xmm14
	mulss xmm0, [rip + .one_fifth]
	addss xmm0, [rip + .one_third]
	mulss xmm0, xmm14
	addss xmm0, [rip + .f_one]

	mulss xmm0, xmm15
	mulss xmm0, [rip + .ln2_inverse_2]

	cvtsi2ss xmm15, rax
	addss xmm0, xmm15 

	ret


.data
.reduce_mask:
	.long 0x3f800000
.f_one:
	.float 1.0
.one_third:
	.float 0.3333333333333333
.one_fifth:
	.float 0.2
.ln2_inverse_2:
	.float 2.8853900817779268

// deg2 approximation
.deg2_co1:
	.float -0.344845
.deg2_co2:
	.float 2.024658
.deg2_co3:
	.float -1.674873

// deg4 approximation
.deg4_co1:
	.float -0.081615808
.deg4_co2:
	.float 0.64514236
.deg4_co3:
	.float -2.1206751
.deg4_co4:
	.float 4.0700908
.deg4_co5:
	.float -2.5128546
