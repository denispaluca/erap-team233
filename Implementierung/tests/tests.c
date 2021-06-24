#include "../src/entropy.h"
#include "../src/log2.h"
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

void print_out(double t)
{

    printf("%f ",t);
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
        "tests/testfiles/data_1_000_000_uni"
    };

    for (size_t i = 0; i < 5; i++)
    {
        struct Handler file;
        file = handle_file(files[i]);

        //float* arr = entropy_c_rand_non_uniform(n);

        print_out(time_calc(it, file.len, file.data, log2f, scalar_entropy));
        print_out(time_calc(it, file.len, file.data, log2approx_deg2, scalar_entropy));
        print_out(time_calc(it, file.len, file.data, log2approx_deg4, scalar_entropy));
        print_out(time_calc(it, file.len, file.data, log2approx_arctanh, scalar_entropy));
        print_out(time_calc(it, file.len, file.data, log2_lookup, scalar_entropy));
        print_out(time_calc(it, file.len, file.data, log2approx_deg2_asm, entropy_asm));
        print_out(time_calc(it, file.len, file.data, log2approx_deg4_asm, entropy_asm));
        print_out(time_calc(it, file.len, file.data, log2approx_arctanh_asm, entropy_asm));
        print_out(time_calc(it, file.len, file.data, log2_lookup_asm, entropy_asm));
        print_out(time_calc_simd(it, file.len, file.data, log2_glibc_simd, simd_entropy));
        print_out(time_calc_simd(it, file.len, file.data, log2approx_deg2_simd, simd_entropy));
        print_out(time_calc_simd(it, file.len, file.data, log2approx_deg4_simd, simd_entropy));
        print_out(time_calc_simd(it, file.len, file.data, log2approx_arctanh_simd, simd_entropy));
        print_out(time_calc_simd(it, file.len, file.data, log2_lookup_simd, simd_entropy));
        print_out(time_calc_simd(it, file.len, file.data, log2approx_deg2_simd_asm, entropy_simd));
        print_out(time_calc_simd(it, file.len, file.data, log2approx_deg4_simd_asm, entropy_simd));
        print_out(time_calc_simd(it, file.len, file.data, log2approx_arctanh_simd_asm, entropy_simd));
        print_out(time_calc_simd(it, file.len, file.data, log2_lookup_simd_asm, entropy_simd));

        printf("\n");
        free(file.data);
    }
}