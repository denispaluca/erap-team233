.intel_syntax noprefix
.global log2_simd

.text

// __m128 log2_simd(__m128 val);
log2_simd:
    sub rsp,0x08
    movaps xmm0,[rip+.Lconst1]
    add rsp,0x08
    ret

    
.align 16
.Lconst1:
    .4byte 0x3F800000
    .4byte 0x3F800000
    .4byte 0x3F800000
    .4byte 0x3F800000

