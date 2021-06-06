.intel_syntax noprefix
.global entropy_asm
// .global log2 // We may wanna use it later
.extern log2f // you can use C libray function in assembly for testing
.text

// float (size_t len, float* data);
entropy_asm:
	// rdi is the length of the array
	// rsi is the pointer to the array

	test rdi,rdi
	je .Lzero
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



// float (float val)
log2:
	ret
