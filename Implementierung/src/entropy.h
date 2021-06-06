#ifndef ENTROPY_H
#define ENTROPY_H

#include <stddef.h>

//===============C functions===================
float scalar_entropy(size_t len,  float* data);
float file_entropy_c(const char* file_name);

//===============Asm Functions==================
extern float entropy_asm (size_t len, float* data);
// extern float log2(float val); // We may wanna use it later

#endif
