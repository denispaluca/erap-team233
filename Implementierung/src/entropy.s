.intel_syntax noprefix
.global entropy_asm
.global entropy_asm_simd
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
	sub rsp,0x08

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
	add rsp,0x08
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

// float (size_t len, float* data);
entropy_asm_simd:
	// rdi is the length of the array
	// rsi is the pointer to the array
	

	// swap values because log2f_simd takes value from rdi register.
	mov rax,rdi
	mov rdi,rsi 
	mov rsi,rax
	// rdi is the pointer to the array
	// rsi is the length of the array.

	cmp rsi,4
	jb .Lscalar

	.Lsimdloop:
	pxor xmm2,xmm2

	movaps xmm1,[rdi]

	call log2f_simd

 	mulps xmm0,xmm1
 	subps xmm2,xmm0
 	add rdi,16
 	sub rsi,4
 	cmp rsi,4
 	ja .Lsimdloop
	.Lscalar:
	//TODO

	movaps xmm1,xmm2
    movaps  xmm0, xmm1
    movaps  xmm2, xmm1
    shufps  xmm2, xmm1, 85
    addss   xmm0, xmm2
    movaps  xmm2, xmm1
    unpckhps        xmm2, xmm1
    shufps  xmm1, xmm1, 255
    addss   xmm0, xmm2
    addss   xmm0, xmm1
	ret


// __m128(float* data) takes the logarithm of first 4 inputs and returns it at xmm0 register.
log2f_simd:
	movups xmm0,[rip+.L1f]
	ret
// return 1.0f
.L1f:
	.8byte 0x3F800000
	.8byte 0x3F800000
	.8byte 0x3F800000
	.8byte 0x3F800000

