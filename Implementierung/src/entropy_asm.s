.intel_syntax noprefix
.global entropy_asm
// .extern log2_asm // We will use it when implemented
.extern log2f // you can use C libray function in assembly for testing

.text
.align 16
// float entropy_asm(size_t len, float* data);
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

	mov r8,rdi
	mov r9,rsi
	movss xmm3,[rip+.Lconstupperlimit]
	movss xmm4,[rip+.Lconstlowerlimit]
	pxor xmm5,xmm5
	movss xmm6,[rip+.Lconst1]
	pxor xmm8,xmm8

	.Lcheck:
		movss xmm7,[r9]

		comiss xmm7,xmm5
		jl .Lerror

		comiss xmm7,xmm6
		jg .Lerror

		addss xmm8,xmm7

		add r9,4
		sub r8,1
		ja .Lcheck
	
	comiss xmm8,xmm3
	jl .Lerror

	comiss xmm8,xmm4
	jg .Lerror
	
	// If I reach here that means the array is a proability distrubition.	

	push r12
	push r13
	push r14

	// store entropy temporarily on xmm2
	pxor xmm2,xmm2 
	
	// preserve rdi,rsi across function calls.
	mov r12,rdi
	mov r13,rsi

	.Lloop:
		// preserve temporary xmm2 register across function call.
		movq r14,xmm2

		movss xmm0,[r13];
		call log2f


		movss xmm1,[r13]
		movq xmm2,r14

		mulss xmm0,xmm1
		subss xmm2,xmm0

		add r13,4
		sub r12,1
		ja .Lloop

	movss xmm0,xmm2

	pop r14
	pop r13
	pop r12
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
