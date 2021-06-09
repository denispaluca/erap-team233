.intel_syntax noprefix
.global entropy_simd
.extern log2_simd

.text
.align 16
// float (size_t len, float* data);
entropy_simd:
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

	mov r8,rdi
	mov r9,rsi
	movss xmm3,[rip+.Lconstupperlimit]
	movss xmm4,[rip+.Lconstlowerlimit]
	pxor xmm5,xmm5
	movaps xmm6,[rip+.Lconst1]
	pxor xmm8,xmm8

    .Lsimdcheck:
        movaps xmm7,[r9]
        movaps xmm9,[r9]

        addps xmm8,xmm7

        // less than operand form. 
        cmpps xmm7,xmm5,1
        pmovmskb eax,xmm7
        test eax,eax
        jnz .Lerror

        // not less than operand form.
        cmpps xmm9,xmm6,5
        pmovmskb eax,xmm9
        test eax,eax
        jnz .Lerror


        add r9,16
        sub r8,4
        ja .Lsimdcheck


    // sum 4 floating points into 1 floating point.
    haddps xmm8,xmm8 
    haddps xmm8,xmm8

    comiss xmm8,xmm3
    ja .Lerror

	comiss xmm8,xmm4
    jb .Lerror

    // swap values because log2f_simd takes value from rdi register.
    mov rax,rdi
    mov rdi,rsi 
    mov rsi,rax


    // rdi is the pointer to the array
    // rsi is the length of the array.

    pxor xmm2,xmm2

    .Lsimdloop:
    
        movaps xmm1,[rdi]

        sub rsp,0x08
        call log2_simd
        add rsp,0x08

        mulps xmm0,xmm1
        subps xmm2,xmm0

        add rdi,16
        sub rsi,4

        ja .Lsimdloop


    movaps xmm0,xmm2
    haddps xmm0,xmm0
    haddps xmm0,xmm0
    xorps xmm0,[rip+.Ltest]
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
.Ltest:
        .long   -2147483648
        .long   0
        .long   0
        .long   0
