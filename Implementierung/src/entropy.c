#include "entropy.h"
#include <stdint.h>

float scalar_entropy(size_t len,  float* data)
{

	float entropy = 0;
	for (size_t i = 0 ; i < len ; ++i)
	{
		entropy -= data[i] * log2approx_arctanh(data[i]);
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


		float entropy = entropy_simd(len, data);
		free(data);
		return entropy;
	}

	// Since entropy is always positive in case of an error returns -1.
	return -1;
}


float simd_entropy(size_t len, float* data,__m128(* log_func) (__m128)) 
{
	// PREREQUISITE len is a multiple of 4.

	__m128 one = _mm_set_ps1(1.0f);
	__m128 zero = _mm_setzero_ps();
	__m128 sum = _mm_setzero_ps();
	__m128 cur,mask;
	uint32_t check_mask;


	//Checking validity of data
	for(size_t i = 0 ; i < len ; i+=4)
	{
		cur = _mm_load_ps(data+i);

		sum += cur;

		mask = _mm_cmplt_ps(cur,zero);

		check_mask = _mm_movemask_epi8( (__m128i)mask);
		if(unlikely(check_mask != 0)) return -1;

		mask = _mm_cmpnle_ps(cur,one);

		check_mask = _mm_movemask_epi8( (__m128i)mask);
		if(unlikely(check_mask != 0)) return -1;
	}
	sum = _mm_hadd_ps(sum,sum);
	sum = _mm_hadd_ps(sum,sum);
	if(sum[0] < LOWER_LIMIT || sum[0] > UPPER_LIMIT)
	{
		return -1;
	}

	sum = _mm_setzero_ps();


	//Calculating entropy
	for(size_t i = 0 ; i < len ; i+=4)
	{
		cur = _mm_load_ps(data+i);
		sum -= cur * log_func(cur);
	}
	sum = _mm_hadd_ps(sum,sum);
	sum = _mm_hadd_ps(sum,sum);
	
	return sum[0];

}

