#include "log2.h"

static void reduce_float_scalar(union num *data, int *exponent)
{

    // Get exponent (omitting sign bit()
    *exponent = data->fix >> 23;

    // Special case for denormal floating numbers
    if (*exponent == 0)
    {
        data->flt *= 0x1P23f;               /* Normalize floating number */
        *exponent = (data->fix >> 23) - 23; /* Recalculate exponent considering exponent used for normalization*/
    }

    // Subtracting bias
    *exponent -= 127;

    data->fix = (data->fix & 0x7FFFFF) | 0x3F800000;
}

// Normal
float log2_deg2_scalar(float x)
{
    union num data = {.flt = x};
    int32_t exponent = 0;

    reduce_float_scalar(&data, &exponent);

    // co1 * x^2 + co2 * x + co3

    // Pipelining
    float y0, y;
    y0 = deg2_co3[0] + (float) exponent;

    y = deg2_co1[0] * data.flt + deg2_co2[0];

    return y * data.flt + y0;
}

float log2_deg4_scalar(float x)
{
    union num data = {.flt = x};
    int32_t exponent = 0;

    reduce_float_scalar(&data, &exponent);

    // co1 * x^4 + co2 * x^3 + co3 * x^2 + co2 * x + co1

    // Pipelining
    float y0, x2, y, z;
    y0 = deg4_co5[0] + (float) exponent;

    x2 = data.flt * data.flt;

    y = deg4_co2[0] * data.flt + deg4_co3[0];
    y = deg4_co1[0] * x2 + y;
    z = deg4_co4[0] * data.flt + y0;

    return y * x2 + z;
}

float log2_artanh_scalar(float x)
{
    union num data = {.flt = x};
    int32_t exponent = 0;

    reduce_float_scalar(&data, &exponent);

    // q = (x - 1) / (x + 1)
    // ( 0.2 * q^5 + 0.3 * q^3 + q ) * (2 / ln2)

    // Pipelining
    float q, q2, y;

    q = (data.flt - 1) / (data.flt + 1);
    q2 = q * q;

    y = one_third[0] + q2 * one_fifth[0];
    y = y * q2 + 1;
    y = y * q * ln2_inverse_2[0];

    return y + (float) exponent;
}

// With Lookup table
float log2_lookup_scalar(float x)
{
    union num data = {.flt = x};
    int32_t exponent = 0;
    int32_t index;

    reduce_float_scalar(&data, &exponent);

    index = (data.fix & 0x7FFFFF) >> (23 - LOG_LOOKUP_TABLE_SIZE);

    return log_lookup_table[index] + (float) exponent;
}
