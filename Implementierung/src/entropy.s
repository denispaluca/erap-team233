.intel_syntax noprefix
.global entropy_asm
.text
// float (size_t len, float* data);
entropy_asm:
	// rdi is the length of the array
	// rsi is the pointer to the array

	test rdi,rdi
	je .Lzer


	pxor xmm2,xmm2 // store the temporary entropy sum.

	// Switch places between rdi and rsi because log2 function takes input from rdi
	// rdi is the pointer to the array
	// rsi is the length of the array
	mov rax,rdi
	mov rdi,rsi
	mov rsi,rdi

	.Lloop

	movss xmm1,[rdi]

	call log2

	mulss xmm0,xmm1
	subss xmm2,xmm0

	add rdi,4
	sub rsi,1
	ja .Lloop

	movsd xmm0,xmm2
	ret

	.Lzero:
		pxor xmm0,xmm0
		ret



// float (float *data)
log2:
	ret

	