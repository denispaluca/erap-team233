#include "entropy.h"

// Asm constants
const float one_f = 1.0;
const float minusone_f = -1.0;
const uint32_t absmask = 0x7FFFFFFF;
const uint32_t signmask = 0x80000000;
const float epsilon_f = __FLT_EPSILON__;
