#include "entropy.h"
#include <stdint.h>

float entropy_scalar(size_t len, const float *data, float (*log2_func)(float))
{
	float entropy = 0;
	float sum = 0;
	
	float c_entropy = 0;
	float c_sum = 0;
	float y = 0, t = 0, tmp = 0;

	for (size_t i = 0; i < len; ++i)
	{

		float x = data[i];

		if (x < 0 || x > 1)
		{
			return -1;
		}

		// needed for log2f
		if (x == 0)
			continue;

		// Kahans Algorithm
		y = x - c_sum;
		t = sum + y;
		c_sum = (t - sum) - y;

		sum = t;

		// Kahans Algorithm
		tmp = - x * log2_func(x);

		y = tmp - c_entropy;
		t = entropy + y;
		c_entropy = (t - entropy) - y;

		entropy = t;
	}

	if (fabs(sum - 1) > __FLT_EPSILON__)
		return -1;

	if (isnan(entropy))
		return -1;

	return entropy;
}

double entropy_precise(size_t len, const float *data)
{
	double sum = 0;
	double entropy = 0;

	double c_entropy = 0;
	double c_sum = 0;
	double y = 0, t = 0, tmp = 0;

	for (size_t i = 0; i < len; ++i)
	{
		float x = data[i];

		if (x < 0 || x > 1)
		{
			return -1;
		}

		// needed for log2f
		if (x == 0)
			continue;


		// Kahans Algorithm
		y = x - c_sum;
		t = sum + y;
		c_sum = (t - sum) - y;

		sum = t;

		// Kahans Algorithm
		tmp = - x * log2(x);

		y = tmp - c_entropy;
		t = entropy + y;
		c_entropy = (t - entropy) - y;

		entropy = t;
	}

	if (fabs(sum - 1) > __FLT_EPSILON__)
		return -1;

	if (isnan(entropy))
		return -1;

	return entropy;
}

// PREREQUISITE:  data should allocated at least len-(4-len % 4) % 4 memory otherwise it is undefined behaviour.
float entropy_simd(size_t len, const float *data, __m128 (*log2_func)(__m128))
{
	__m128 one = _mm_set_ps1(1.0f);
	__m128 zero = _mm_setzero_ps();
	__m128 sum = _mm_setzero_ps();
	__m128 x, mask;
	uint32_t check_mask;
	__m128 entropy = _mm_setzero_ps();


	__m128 c_entropy = _mm_setzero_ps();
	__m128 c_sum = _mm_setzero_ps();
	__m128 y = _mm_setzero_ps();
	__m128 t = _mm_setzero_ps();
	__m128 tmp = _mm_setzero_ps();

	//Calculating entropy
	for (size_t i = 0; i < len; i += 4)
	{
		x = _mm_load_ps(data + i);

		// Check if numbers between 0  and 1
		mask = _mm_cmplt_ps(x, zero);

		check_mask = _mm_movemask_epi8((__m128i)mask);

		if (unlikely(check_mask != 0))
			return -1;

		mask = _mm_cmpnle_ps(x, one);

		check_mask = _mm_movemask_epi8((__m128i)mask);

		if (unlikely(check_mask != 0))
			return -1;
			

		// Kahans Algorithm
		y = x - c_sum;
		t = sum + y;
		c_sum = (t - sum) - y;

		sum = t;

		// Kahans Algorithm
		tmp = - x * log2_func(x);

		y = tmp - c_entropy;
		t = entropy + y;
		c_entropy = (t - entropy) - y;

		entropy = t;
	}

	sum = _mm_hadd_ps(sum, sum);
	sum = _mm_hadd_ps(sum, sum);

	if (fabs(sum[0] - 1) > __FLT_EPSILON__)
	{
		return -1;
	}

	entropy = _mm_hadd_ps(entropy, entropy);
	entropy = _mm_hadd_ps(entropy, entropy);

	return entropy[0];
}
