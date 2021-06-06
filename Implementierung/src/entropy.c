#include "entropy.h"

float scalar_entropy(size_t len,  float* data)
{

	float entropy = 0;
	for (size_t i = 0 ; i < len ; ++i)
	{
		entropy -= data[i] * log2f(data[i]);
	}
	return entropy;
}