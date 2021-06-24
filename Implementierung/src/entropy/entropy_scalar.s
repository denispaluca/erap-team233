.intel_syntax noprefix
.global entropy_scalar_asm

.text
.align 16
//float+ (size_t len, float* data, float (* log2_func) (float));
entropy_scalar_asm:
	// rdi is the length of the array
	// rsi is the pointer to the array
	// rdx is the log2 function

	test rdi,rdi
	je .Lerror

	/*
		rdi = length
		rsi = pointer to array
		rdx = function
		xmm0 = current element
		xmm1 = current element
		xmm2 = entropy
		xmm3 = sum
		xmm4 = c_sum(Kahans Algorithm)
		xmm5 = c_entropy(Kahans Algorithm)
		xmm6 = 0 (Checking if number is negative)
		xmm7 = 1 (Checking if numnber is bigger than 1)
	*/
	
	pxor xmm2,xmm2
	pxor xmm3,xmm3
	pxor xmm4,xmm4
	pxor xmm5,xmm5
	pxor xmm6,xmm6
	movss xmm7,[rip+.Lconst1]

	.Lloop:

		movss xmm0,[rsi];
		movss xmm1,[rsi]
		
		// Checking x < 0
		comiss xmm0,xmm6
		jb .Lerror

		// Checking x > 1
		comiss xmm0,xmm7
		ja .Lerror

		sub rsp,0x08
		call rdx
		add rsp,0x08
		/*
			Kahans Algorithm
			xmm0 = log2f(x)
			xmm1 = x
			xmm2 = entropy
			xmm3 = sum
			xmm4 = c_sum
			xmm5 = c_entropy
			xmm12 = y
			xmm11 = t
		*/

		// Sum part
		movss xmm12,xmm1 
		movss xmm11,xmm3 

		subss xmm12,xmm4 
		addss xmm11,xmm12

		movss xmm4,xmm11 
		subss xmm4,xmm3 
		subss xmm4,xmm12

		movss xmm3,xmm11 

		//Entropy part
		mulss xmm0,xmm1

		movss xmm12,xmm0 
		movss xmm11,xmm2 

		subss xmm12,xmm5 
		addss xmm11,xmm12 

		movss xmm5,xmm11 
		subss xmm5,xmm2 
		subss xmm5,xmm12

		movss xmm2,xmm11

		add rsi,4
		sub rdi,1
	 	ja .Lloop

	movss xmm9,[rip+.Lfepsilon]
	movss xmm10,[rip+.Land]
	movss xmm11,[rip+.Lxor]

	// checking if |sum-1| < epsilon
	subss xmm3,xmm7
	andps xmm3,xmm10

	comiss xmm3,xmm9
	ja .Lerror

	// Since in the loop I didnt subtract but added I take the aboslute value.
	movss xmm0,xmm2
	xorps xmm0,xmm11
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
.Land:
	.4byte 0x7FFFFFFF
	.4byte 0x00000000
	.4byte 0x00000000 
	.4byte 0x00000000
.align 16
.Lxor:
	.long 0x80000000
    .long 0x00000000
    .long 0x00000000
    .long 0x00000000
.align 16
.Lfepsilon:
	.4byte 0x34000000
	
