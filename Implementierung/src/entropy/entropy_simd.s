.intel_syntax noprefix
.global entropy_simd_asm

.text
.align 16
// PREREQUISITE:  data must allocate 4*k size otherwise it is undefined behaviour.
// float(size_t len, float* data, __m128(* log_func) (__m128));
entropy_simd_asm:
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
        xmm8 = current element
        xmm9 = current element
	*/

    pxor xmm2,xmm2
    pxor xmm3,xmm3
    pxor xmm4,xmm4
    pxor xmm5,xmm5
    pxor xmm6,xmm6

    // one_packed defined in log2 constants
    movaps xmm7,[rip + one_packed]

    .Lloop:
        movaps xmm0,[rsi]
        movaps xmm1,[rsi]
        movaps xmm8,[rsi]
        movaps xmm9,[rsi] 

        // Checking x < 0 
        cmpps xmm8,xmm6,1
        pmovmskb eax,xmm8
        test eax,eax
        jnz .Lerror

		// Checking x > 1
        cmpps xmm9,xmm7,6
        pmovmskb eax,xmm9
        test eax,eax
        jnz .Lerror
       
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
        movaps xmm12,xmm1 
		movaps xmm11,xmm3 

		subps xmm12,xmm4 
		addps xmm11,xmm12

		movaps xmm4,xmm11 
		subps xmm4,xmm3 
		subps xmm4,xmm12

		movaps xmm3,xmm11 

        // Entropy Part
        mulps xmm0,xmm1

        movaps xmm12,xmm0 
		movaps xmm11,xmm2 

		subps xmm12,xmm5 
		addps xmm11,xmm12 

		movaps xmm5,xmm11 
		subps xmm5,xmm2 
		subps xmm5,xmm12

		movaps xmm2,xmm11

        add rsi,16
        sub rdi,4
        ja .Lloop

    movss xmm9,[rip + epsilon_f]
    movss xmm10,[rip + cmpmask]
    movss xmm11,[rip + absmask]

    haddps xmm3,xmm3 
    haddps xmm3,xmm3

    // checking if |sum-1| < epsilon
    subss xmm3,xmm7
	andps xmm3,xmm10

	comiss xmm3,xmm9
	ja .Lerror

    movaps xmm0,xmm2
    haddps xmm0,xmm0
    haddps xmm0,xmm0

    xorps xmm0,xmm11
    ret
    .Lerror:
        movss xmm0,[rip + minusone_f]
		ret
