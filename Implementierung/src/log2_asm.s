.intel_syntax noprefix
.global log2approx_deg2_asm
.global log2approx_deg4_asm

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
	// log_2(val) = () TODO write later
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


.data
.reduce_mask:
	.int 0x3f800000

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