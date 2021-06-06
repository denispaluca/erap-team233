.intel_syntax noprefix
.global entropy_simd
// .extern log2 // We will use it when implemented
.extern log2f_simd

.text

// float (size_t len, float* data);
entropy_simd:
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