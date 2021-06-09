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


float log2_lookup(unsigned n, float* table, float x){

}
