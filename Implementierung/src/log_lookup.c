//
// Created by denis on 09.06.21.
//
#include <math.h>

void init_table(unsigned n, float* table)
{
    int steps = (1 << n);
    float x = 1.0f + (1.0f / (float)( 1 <<(n + 1) ));
    float stepSize = 1.0f / (float)steps;
    for(int i = 0;  i < steps;  i++ )
    {
        table[i] = log2f(x);
        x += stepSize;
    }
}

union data {float flt; int fix;};
float log2_lookup(unsigned n, float* table, float x){
    union data ix;
    ix.flt = x;
    int exponent, index;
    exponent = (ix.fix >> 23) - 127;
    index = (ix.fix & 0x7FFFFF) >> (23 - n);

    return table[index] + exponent;
}
