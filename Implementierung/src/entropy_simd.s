.intel_syntax noprefix
.global entropy_simd
// .extern log2 // We will use it when implemented
.extern log2f

.text

// float entropy_simd(size_t len, float* data);
entropy_simd:
    ret
