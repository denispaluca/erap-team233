#include "entropy.h"
#include <stdint.h>
float log2approx_deg2(float);


float scalar_entropy(size_t len,  float* data)
{

	float entropy = 0;
	for (size_t i = 0 ; i < len ; ++i)
	{
		entropy -= data[i] * log2f(data[i]);
	}
	return entropy;
}
float file_entropy_c(const char* file_name)
{
	size_t len = size_file(file_name);
	float* data = NULL;
	if (len != 0)
	{
		data = read_file(len, file_name);
	}
	if (data != NULL)
	{


		float entropy = scalar_entropy(len, data);
		free(data);
		return entropy;
	}

	// Since entropy is always positive in case of an error returns -1.
	return -1;
}



float log2approx_deg2(float x){
    uint32_t ix, iz;
    int exponent;
    float m, y, a1 = -0.344845f, a2 = 2.024658f, a3 = 1.674873f;
    ix = *(uint32_t *)&x;
    exponent = ((int32_t) ix >> 23) - 127;
    iz = (ix & 0x7fffff) + 0x3f800000;
    m = *(float *) &iz;

    y = a1*m*m + a2*m + (exponent - a3);
    return y;
}