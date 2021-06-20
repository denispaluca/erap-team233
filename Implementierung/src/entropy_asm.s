.intel_syntax noprefix
.global entropy_asm
// .extern log2_asm // We will use it when implemented
.extern log2f // you can use C libray function in assembly for testing

.text
.align 16
//float+ (size_t len, float* data, float (* log2_func) (float));
entropy_asm:
	// rdi is the length of the array
	// rsi is the pointer to the array

	test rdi,rdi
	je .Lzero


	// First checking if the array has the properties of probability distrubition.
	/*
		r8 = len
		r9 = data
		xmm3 = 1.00005 Upper limit for total sum
		xmm4 = 0.99995 Lower limit for total sum
		xmm5 = 0.0
		xmm6 = 1.0
		xmm7 = current element
		xmm8 = total sum
	*/
	cvtsi2ss xmm9,rdi
	mulss xmm9,[rip+.Lconst1eminus7]
	mov r8,rdi
	mov r9,rsi
	movss xmm3,[rip+.Lconstupperlimit]
	addss xmm3,xmm9
	movss xmm4,[rip+.Lconstlowerlimit]
	subss xmm4,xmm9
	pxor xmm5,xmm5
	movss xmm6,[rip+.Lconst1]
	pxor xmm8,xmm8

	.Lcheck:
		movss xmm7,[r9]

		addss xmm8,xmm7

		comiss xmm7,xmm5
		jb .Lerror

		comiss xmm7,xmm6
		ja .Lerror


		add r9,4
		sub r8,1
		ja .Lcheck
	
	comiss xmm8,xmm3
	ja .Lerror

	comiss xmm8,xmm4
	jb .Lerror
	
	// If I reach here that means the array is a proability distrubition.	


	// store entropy temporarily on xmm2
	pxor xmm2,xmm2 
	

	.Lloop:

		movss xmm0,[rsi];

		sub rsp,0x08
		call rdx
		add rsp,0x08

		movss xmm1,[rsi]

		mulss xmm0,xmm1
		subss xmm2,xmm0

		add rsi,4
		sub rdi,1
	 	ja .Lloop

	movss xmm0,xmm2
	ret

	.Lzero:
		pxor xmm0,xmm0
		ret
	.Lerror:
		movss xmm0,[rip+.Lconstminus1]
		ret

// Constants
.align 16
.Lconstminus1:
	.4byte 0xBF800000
.align 16
.Lconst1:
	.4byte 0x3F800000
.align 16
.Lconstupperlimit:
	.4byte 0x3F80002A
.align 16
.Lconstlowerlimit:
	.4byte 0x3F7FFFAC
.align 16
.Lconst1eminus7:
	.4byte 0x33D6BF95
