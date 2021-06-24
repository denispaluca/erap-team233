#include "entropy.h"
#include <stdint.h>

float scalar_entropy(size_t len, const float* data, float (* log2_func) (float))
{
    const float error_margin = len*1e-7;
	float entropy = 0;
	float sum = 0;
	for (size_t i = 0 ; i < len ; ++i)
	{
	    float x = data[i];
	    if(x < 0 || x > 1){
	        return -1;
	    }

		// needed for log2f
		if (x == 0) continue;

	    sum += x;
		entropy -= x * log2_func(x);
	}

	if(sum > (UPPER_LIMIT + error_margin) || sum < (LOWER_LIMIT - error_margin))
	    return -1;
	if(isnan(entropy)) return -1;
	return entropy;
}
double precise_entropy(size_t len, const float* data){
	const double error_margin = len*1e-7;
	double sum = 0;
	double entropy = 0;
	for (size_t i = 0 ; i < len ; ++i)
	{
		double x = data[i];
	    if(x < 0 || x > 1){
	        return -1;
	    }
		sum += x;
        if(x == 0.0f) continue;
		entropy -= x * log2(x);
	}
	if(sum > (UPPER_LIMIT + error_margin) || sum < (LOWER_LIMIT - error_margin))
	    return -1;
	if(isnan(entropy)) return -1;
	return entropy;
}

// PREREQUISITE:  data should allocated at least len-(4-len % 4) % 4 memory otherwise it is undefined behaviour.
float simd_entropy(size_t len, const float* data, __m128(* log2_func) (__m128)) 
{
    const float error_margin = len*1e-7;
	__m128 one = _mm_set_ps1(1.0f);
	__m128 zero = _mm_setzero_ps();
	__m128 sum = _mm_setzero_ps();
	__m128 cur,mask;
	uint32_t check_mask;
	__m128 entropy = _mm_setzero_ps();

	//Calculating entropy
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
		entropy -= cur * log2_func(cur);
	}

	sum = _mm_hadd_ps(sum,sum);
	sum = _mm_hadd_ps(sum,sum);
	if(sum[0] < (LOWER_LIMIT - error_margin) || sum[0] > (UPPER_LIMIT + error_margin))
	{
		return -1;
	}
	entropy = _mm_hadd_ps(entropy,entropy);
	entropy = _mm_hadd_ps(entropy,entropy);
	
	return entropy[0];
}
