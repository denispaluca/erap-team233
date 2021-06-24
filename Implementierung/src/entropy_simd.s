.intel_syntax noprefix
.global entropy_simd_asm

.text
.align 16
// float(size_t len, float* data, __m128(* log_func) (__m128));
entropy_simd_asm:
    // rdi is the length of the array
    // rsi is the pointer to the array
    // PREREQUISITE:  len must be a multiple of 4.

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
        xmm9 = current element
	*/
    cvtsi2ss xmm9,rdi
	mulss xmm9,[rip+.Lconst1eminus7]
	movss xmm3,[rip+.Lconstupperlimit]
	movss xmm4,[rip+.Lconstlowerlimit]
    addss xmm3,xmm9
    subss xmm4,xmm9
	pxor xmm5,xmm5
	movaps xmm6,[rip+.Lconst1]
	pxor xmm8,xmm8
    pxor xmm2,xmm2

    .Lsimdloop:
        movaps xmm7,[rsi]
        movaps xmm9,[rsi] 
        movaps xmm1,[rsi]
        movaps xmm0,[rsi]

        addps xmm8,xmm7

        // less than operand form. 
        cmpps xmm7,xmm5,1
        pmovmskb eax,xmm7
        test eax,eax
        jnz .Lerror

        // not less or equal operand form.
        cmpps xmm9,xmm6,6
        pmovmskb eax,xmm9
        test eax,eax
        jnz .Lerror


        sub rsp,0x08
        call rdx
        add rsp,0x08

        mulps xmm0,xmm1
        subps xmm2,xmm0

        add rsi,16
        sub rdi,4

        ja .Lsimdloop

    haddps xmm8,xmm8 
    haddps xmm8,xmm8

    comiss xmm8,xmm3
    ja .Lerror

	comiss xmm8,xmm4
    jb .Lerror

    movaps xmm0,xmm2
    haddps xmm0,xmm0
    haddps xmm0,xmm0
    ret
    .Lerror:
        movaps xmm0,[rip+.Lconstminus1]
		ret


.align 16
.Lconstminus1:
	.4byte 0xBF800000
.align 16
.Lconst1:
	.4byte 0x3F800000
    .4byte 0x3F800000
    .4byte 0x3F800000
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

