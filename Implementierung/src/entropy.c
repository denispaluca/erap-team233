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
