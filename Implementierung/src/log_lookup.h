//
// Created by denis on 09.06.21.
//

#ifndef IMPLEMENTIERUNG_LOG_LOOKUP_H
#define IMPLEMENTIERUNG_LOG_LOOKUP_H
#include <immintrin.h>

void init_table(unsigned n, float* table);
float log2_lookup(unsigned n, float* table, float x);
__mm128 log2_lookup_simd(unsigned n, float* table, __mm128 x);
#endif //IMPLEMENTIERUNG_LOG_LOOKUP_H
