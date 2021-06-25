#include "../src/entropy/entropy.h"
#include <locale.h>
#pragma GCC diagnostic ignored "-Wformat-security"

double time_calc(size_t it, size_t n, const float *arr, float (*log2_func)(float), float (*entropy)(size_t, const float *, float (*)(float)))
{

    struct timespec start, end;
    clock_gettime(CLOCK_MONOTONIC, &start);

    for (size_t i = 0; i < it; i++)
    {
        entropy(n, arr, log2_func);
    }

    clock_gettime(CLOCK_MONOTONIC, &end);

    double time_secs = (end.tv_sec - start.tv_sec) + 1e-9 * (end.tv_nsec - start.tv_nsec);
    return time_secs;
}

double time_calc_simd(size_t it, size_t n, const float *arr, __m128 (*log2_func)(__m128), float (*entropy)(size_t, const float *, __m128 (*)(__m128)))
{

    struct timespec start, end;
    clock_gettime(CLOCK_MONOTONIC, &start);

    for (size_t i = 0; i < it; i++)
    {
        entropy(n, arr, log2_func);
    }

    clock_gettime(CLOCK_MONOTONIC, &end);

    double time_secs = (end.tv_sec - start.tv_sec) + 1e-9 * (end.tv_nsec - start.tv_nsec);
    return time_secs;
}

double accuracy_diff(size_t n, const float *arr, float (*log2_func)(float), float (*entropy)(size_t, const float *, float (*)(float)))
{
    double precise = entropy_precise(n, arr);
    float actual = entropy(n, arr, log2_func);

    double diff = fabs(precise - (double)actual);

    return diff;
}
double accuracy_diff_simd(size_t n, const float *arr, __m128 (*log2_func)(__m128), float (*entropy)(size_t, const float *, __m128 (*)(__m128)))
{
    double precise = entropy_precise(n, arr);
    float actual = entropy(n, arr, log2_func);

    double diff = fabs(precise - (double)actual);

    return diff;
}

void print_out(double t)
{

    printf("%f ", t);
}

int main()
{
    size_t it = 1000;
    size_t n = 10;

    char *files[] = {
        "tests/testfiles/data_100_000_non-uni",
        "tests/testfiles/data_100_000_uni",
        "tests/testfiles/data_500_000_non-uni",
        "tests/testfiles/data_1_000_000_non-uni",
        "tests/testfiles/data_1_000_000_uni"};

    // for (size_t i = 0; i < 5; i++)
    // {
    //     struct Handler file;
    //     file = handle_file(files[i]);

    //     //float* arr = entropy_c_rand_non_uniform(n);

    //     print_out(time_calc(it, file.len, file.data, log2f, entropy_scalar));
    //     print_out(time_calc(it, file.len, file.data, log2_deg2_scalar, entropy_scalar));
    //     print_out(time_calc(it, file.len, file.data, log2_deg4_scalar, entropy_scalar));
    //     print_out(time_calc(it, file.len, file.data, log2_artanh_scalar, entropy_scalar));
    //     print_out(time_calc(it, file.len, file.data, log2_lookup_scalar, entropy_scalar));
    //     print_out(time_calc(it, file.len, file.data, log2_deg2_scalar_asm, entropy_scalar_asm));
    //     print_out(time_calc(it, file.len, file.data, log2_deg4_scalar_asm, entropy_scalar_asm));
    //     print_out(time_calc(it, file.len, file.data, log2_artanh_scalar_asm, entropy_scalar_asm));
    //     print_out(time_calc(it, file.len, file.data, log2_lookup_scalar_asm, entropy_scalar_asm));
    //     print_out(time_calc_simd(it, file.len, file.data, log2_glibc_simd, entropy_simd));
    //     print_out(time_calc_simd(it, file.len, file.data, log2_deg2_simd, entropy_simd));
    //     print_out(time_calc_simd(it, file.len, file.data, log2_deg4_simd, entropy_simd));
    //     print_out(time_calc_simd(it, file.len, file.data, log2_artanh_simd, entropy_simd));
    //     print_out(time_calc_simd(it, file.len, file.data, log2_lookup_simd, entropy_simd));
    //     print_out(time_calc_simd(it, file.len, file.data, log2_deg2_simd_asm, entropy_simd_asm));
    //     print_out(time_calc_simd(it, file.len, file.data, log2_deg4_simd_asm, entropy_simd_asm));
    //     print_out(time_calc_simd(it, file.len, file.data, log2_artanh_simd_asm, entropy_simd_asm));
    //     print_out(time_calc_simd(it, file.len, file.data, log2_lookup_simd_asm, entropy_simd_asm));

    //     printf("\n");
    //     free(file.data);
    // }
    double array[18];
    for (size_t i = 0; i < 18; ++i)
    {
        array[i] = 0;
    }
    printf("Calculating \n");
    size_t iterations = 25000;
    for (size_t i = 1; i <= iterations; ++i)
    {
        float *data = entropy_urandom_non_uniform(i);
        array[0] += accuracy_diff(i, data, log2f, entropy_scalar);
        array[1] += accuracy_diff(i, data, log2_deg2_scalar, entropy_scalar);
        array[2] += accuracy_diff(i, data, log2_deg4_scalar, entropy_scalar);
        array[3] += accuracy_diff(i, data, log2_artanh_scalar, entropy_scalar);
        array[4] += accuracy_diff(i, data, log2_lookup_scalar, entropy_scalar);
        array[5] += accuracy_diff(i, data, log2_deg2_scalar_asm, entropy_scalar_asm);
        array[6] += accuracy_diff(i, data, log2_deg4_scalar_asm, entropy_scalar_asm);
        array[7] += accuracy_diff(i, data, log2_artanh_scalar_asm, entropy_scalar_asm);
        array[8] += accuracy_diff(i, data, log2_lookup_scalar_asm, entropy_scalar_asm);
        array[9] += accuracy_diff_simd(i, data, log2_glibc_simd, entropy_simd);
        array[10] += accuracy_diff_simd(i, data, log2_deg2_simd, entropy_simd);
        array[11] += accuracy_diff_simd(i, data, log2_deg4_simd, entropy_simd);
        array[12] += accuracy_diff_simd(i, data, log2_artanh_simd, entropy_simd);
        array[13] += accuracy_diff_simd(i, data, log2_lookup_simd, entropy_simd);
        array[14] += accuracy_diff_simd(i, data, log2_deg2_simd_asm, entropy_simd_asm);
        array[15] += accuracy_diff_simd(i, data, log2_deg4_simd_asm, entropy_simd_asm);
        array[16] += accuracy_diff_simd(i, data, log2_artanh_simd_asm, entropy_simd_asm);
        array[17] += accuracy_diff_simd(i, data, log2_lookup_simd_asm, entropy_simd_asm);
        free(data);
    }
    for (size_t i = 0; i < 18; ++i)
    {
        double result = array[i] / iterations;
        printf("%f \n", result);
    }
}