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

    // swap values because log2f_simd takes value from rdi register.
    mov rax,rdi
    mov rdi,rsi 
    mov rsi,rax
    // rdi is the pointer to the array
    // rsi is the length of the array.





    pxor xmm2,xmm2

    .Lsimdloop:
    
    movaps xmm1,[rdi]

    call log2_simd

    mulps xmm0,xmm1
    subps xmm2,xmm0

    add rdi,16
    sub rsi,4


    jae .Lsimdloop


    movaps xmm0,xmm2
    haddps xmm0,xmm0
    haddps xmm0,xmm0
    
    ret

