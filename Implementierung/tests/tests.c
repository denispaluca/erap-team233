#include "../src/entropy/entropy.h"
#include <locale.h>

static double time_calc(size_t it, size_t n, const float *arr, float (*log2_func)(float), float (*entropy)(size_t, const float *, float (*)(float)))
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

static double time_calc_simd(size_t it, size_t n, const float *arr, __m128 (*log2_func)(__m128), float (*entropy)(size_t, const float *, __m128 (*)(__m128)))
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

static double accuracy_diff(size_t n, const float *arr, float (*log2_func)(float), float (*entropy)(size_t, const float *, float (*)(float)))
{
    double precise = entropy_precise(n, arr);
    float actual = entropy(n, arr, log2_func);

    double diff = fabs(precise - (double)actual);

    return diff;
}

static double accuracy_diff_simd(size_t n, const float *arr, __m128 (*log2_func)(__m128), float (*entropy)(size_t, const float *, __m128 (*)(__m128)))
{
    double precise = entropy_precise(n, arr);
    float actual = entropy(n, arr, log2_func);

    double diff = fabs(precise - (double)actual);

    return diff;
}

static const char *files[] = {
    "tests/testfiles/data_100_000_non-uni",
    "tests/testfiles/data_100_000_uni",
    "tests/testfiles/data_500_000_non-uni",
    "tests/testfiles/data_1_000_000_non-uni",
    "tests/testfiles/data_1_000_000_uni"};

void test_performance(size_t it)
{

    for (size_t i = 0; i < 5; i++)
    {
        struct Handler file;
        file.data = NULL;
        file.len = 0;
        file.status = -1;

        file = handle_file(files[i]);

        if (file.data != NULL)
        {
            printf("Performance benchmark for the file: %s \n", files[i]);
            double time = 0;
            time = time_calc(it, file.len, file.data, log2f, entropy_scalar);
            printf("LOG2F Calculation took: %*s%f seconds\n", 14, "", time);
            time = time_calc(it, file.len, file.data, log2_deg2_scalar, entropy_scalar);
            printf("DEG2 Calculation took: %*s%f seconds\n", 15, "", time);

            time = time_calc(it, file.len, file.data, log2_deg4_scalar, entropy_scalar);
            printf("DEG4 Calculation took: %*s%f seconds\n", 15, "", time);

            time = time_calc(it, file.len, file.data, log2_artanh_scalar, entropy_scalar);
            printf("ARTANH Calculation took: %*s%f seconds\n", 13, "", time);

            time = time_calc(it, file.len, file.data, log2_lookup_scalar, entropy_scalar);
            printf("LOOKUP Calculation took: %*s%f seconds\n", 13, "", time);

            time = time_calc(it, file.len, file.data, log2_deg2_scalar_asm, entropy_scalar_asm);
            printf("ASM DEG2 Calculation took: %*s%f seconds\n", 11, "", time);

            time = time_calc(it, file.len, file.data, log2_deg4_scalar_asm, entropy_scalar_asm);
            printf("ASM DEG4 Calculation took: %*s%f seconds\n", 11, "", time);

            time = time_calc(it, file.len, file.data, log2_artanh_scalar_asm, entropy_scalar_asm);
            printf("ASM ARTANH Calculation took: %*s%f seconds\n", 9, "", time);

            time = time_calc(it, file.len, file.data, log2_lookup_scalar_asm, entropy_scalar_asm);
            printf("ASM LOOKUP Calculation took: %*s%f seconds\n", 9, "", time);

            time = time_calc_simd(it, file.len, file.data, log2_glibc_simd, entropy_simd);
            printf("SIMD GLIBC Calculation took: %*s%f seconds\n", 9, "", time);

            time = time_calc_simd(it, file.len, file.data, log2_deg2_simd, entropy_simd);
            printf("SIMD DEG2 Calculation took: %*s%f seconds\n", 10, "", time);

            time = time_calc_simd(it, file.len, file.data, log2_deg4_simd, entropy_simd);
            printf("SIMD DEG4 Calculation took: %*s%f seconds\n", 10, "", time);

            time = time_calc_simd(it, file.len, file.data, log2_artanh_simd, entropy_simd);
            printf("SIMD ARTANH Calculation took: %*s%f seconds\n", 8, "", time);

            time = time_calc_simd(it, file.len, file.data, log2_lookup_simd, entropy_simd);
            printf("SIMD LOOKUP Calculation took: %*s%f seconds\n", 8, "", time);

            time = time_calc_simd(it, file.len, file.data, log2_deg2_simd_asm, entropy_simd_asm);
            printf("ASM SIMD DEG2 Calculation took: %*s%f seconds\n", 6, "", time);

            time = time_calc_simd(it, file.len, file.data, log2_deg4_simd_asm, entropy_simd_asm);
            printf("ASM SIMD DEG4 Calculation took: %*s%f seconds\n", 6, "", time);

            time = time_calc_simd(it, file.len, file.data, log2_artanh_simd_asm, entropy_simd_asm);
            printf("ASM SIMD ARTANH Calculation took: %*s%f seconds\n", 4, "", time);

            time = time_calc_simd(it, file.len, file.data, log2_lookup_simd_asm, entropy_simd_asm);
            printf("ASM SIMD LOOKUP Calculation took: %*s%f seconds\n", 4, "", time);

            printf("\n");
            free(file.data);
        }
    }
}

void test_accuracy()
{
    double array[18];

    for (size_t i = 0; i < 18; ++i)
    {
        array[i] = 0;
    }

    uint32_t success = 0;

    for (size_t i = 0; i < 5; ++i)
    {

        struct Handler file;
        file.data = NULL;
        file.len = 0;
        file.status = -1;

        file = handle_file(files[i]);
        if (file.data != NULL)
        {
            array[0] += accuracy_diff(file.len, file.data, log2f, entropy_scalar);
            array[1] += accuracy_diff(file.len, file.data, log2_deg2_scalar, entropy_scalar);
            array[2] += accuracy_diff(file.len, file.data, log2_deg4_scalar, entropy_scalar);
            array[3] += accuracy_diff(file.len, file.data, log2_artanh_scalar, entropy_scalar);
            array[4] += accuracy_diff(file.len, file.data, log2_lookup_scalar, entropy_scalar);
            array[5] += accuracy_diff(file.len, file.data, log2_deg2_scalar_asm, entropy_scalar_asm);
            array[6] += accuracy_diff(file.len, file.data, log2_deg4_scalar_asm, entropy_scalar_asm);
            array[7] += accuracy_diff(file.len, file.data, log2_artanh_scalar_asm, entropy_scalar_asm);
            array[8] += accuracy_diff(file.len, file.data, log2_lookup_scalar_asm, entropy_scalar_asm);
            array[9] += accuracy_diff_simd(file.len, file.data, log2_glibc_simd, entropy_simd);
            array[10] += accuracy_diff_simd(file.len, file.data, log2_deg2_simd, entropy_simd);
            array[11] += accuracy_diff_simd(file.len, file.data, log2_deg4_simd, entropy_simd);
            array[12] += accuracy_diff_simd(file.len, file.data, log2_artanh_simd, entropy_simd);
            array[13] += accuracy_diff_simd(file.len, file.data, log2_lookup_simd, entropy_simd);
            array[14] += accuracy_diff_simd(file.len, file.data, log2_deg2_simd_asm, entropy_simd_asm);
            array[15] += accuracy_diff_simd(file.len, file.data, log2_deg4_simd_asm, entropy_simd_asm);
            array[16] += accuracy_diff_simd(file.len, file.data, log2_artanh_simd_asm, entropy_simd_asm);
            array[17] += accuracy_diff_simd(file.len, file.data, log2_lookup_simd_asm, entropy_simd_asm);
            ++success;
            free(file.data);
        }
    }
    if (success != 0)
    {
        double abs_mistake = 0;
        abs_mistake = array[0] / success;
        printf("LOG2F Absolute Mistake:%*s%f\n", 15, "", abs_mistake);

        abs_mistake = array[1] / success;
        printf("DEG2 Absolute Mistake:%*s%f\n", 16, "", abs_mistake);

        abs_mistake = array[2] / success;
        printf("DEG4 Absolute Mistake:%*s%f\n", 16, "", abs_mistake);

        abs_mistake = array[3] / success;
        printf("ARTANH Absolute Mistake:%*s%f\n", 14, "", abs_mistake);

        abs_mistake = array[4] / success;
        printf("LOOKUP Absolute Mistake:%*s%f\n", 14, "", abs_mistake);

        abs_mistake = array[5] / success;
        printf("ASM DEG2 Absolute Mistake:%*s%f\n", 12, "", abs_mistake);

        abs_mistake = array[6] / success;
        printf("ASM DEG4 Absolute Mistake:%*s%f\n", 12, "", abs_mistake);

        abs_mistake = array[7] / success;
        printf("ASM ARTANH Absolute Mistake:%*s%f\n", 10, "", abs_mistake);

        abs_mistake = array[8] / success;
        printf("ASM LOOKUP Absolute Mistake:%*s%f\n", 10, "", abs_mistake);

        abs_mistake = array[9] / success;
        printf("SIMD GLIBC Absolute Mistake:%*s%f\n", 10, "", abs_mistake);

        abs_mistake = array[10] / success;
        printf("SIMD DEG2 Absolute Mistake:%*s%f\n", 11, "", abs_mistake);

        abs_mistake = array[11] / success;
        printf("SIMD DEG4 Absolute Mistake:%*s%f\n", 11, "", abs_mistake);

        abs_mistake = array[12] / success;
        printf("SIMD ARTANH Absolute Mistake:%*s%f\n", 9, "", abs_mistake);

        abs_mistake = array[13] / success;
        printf("SIMD LOOKUP Absolute Mistake:%*s%f\n", 9, "", abs_mistake);

        abs_mistake = array[14] / success;
        printf("ASM SIMD DEG2 Absolute Mistake:%*s%f\n", 7, "", abs_mistake);

        abs_mistake = array[15] / success;
        printf("ASM SIMD DEG4 Absolute Mistake:%*s%f\n", 7, "", abs_mistake);

        abs_mistake = array[16] / success;
        printf("ASM SIMD ARTANH Absolute Mistake:%*s%f\n", 5, "", abs_mistake);

        abs_mistake = array[17] / success;
        printf("ASM SIMD LOOKUP Absolute Mistake:%*s%f\n", 5, "", abs_mistake);
    }
    else
    {
        printf("No entropy was calculated make sure that test files exist. \n");
    }
}
