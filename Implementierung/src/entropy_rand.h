#ifndef ENTROPY_RAND_H
#define ENTROPY_RAND_H
#include <time.h>
#include <stdio.h>
#include <stdlib.h>
#define likely(x)       __builtin_expect((x),1)
#define unlikely(x)     __builtin_expect((x),0)

float entropy_c_rand(size_t len);
float entropy_c_urandom(size_t len);

#endif 