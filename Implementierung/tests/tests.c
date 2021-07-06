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

static const char* dir = "tests/testfiles/";

static const char *files_performance[] = {
    "data_100_000_non-uni",
    "data_100_000_uni",
    "data_500_000_non-uni",
    "data_1_000_000_non-uni",
    "data_1_000_000_uni"
    };
static const char *files_accuracy[] = {
    "data_500_000_non-uni",
    "data_500_000_non-uni-2",
    "data_500_000_non-uni-3",
    "data_500_000_non-uni-4",
    "data_500_000_non-uni-5"
    };

static const char* seperator = "------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------";

void test_performance(size_t it)
{

    puts("\n");
    puts("Executing tests for Performance...(All results are in SECONDS)");
    puts(seperator);
    printf("%-23s| %-16s| %-16s| %-16s| %-16s| %-16s| %-16s| %-16s| %-16s| %-16s\n",
    "FILE",
    "LOG2F", 
    "DEG2", 
    "DEG4", 
    "ARTANH", 
    "LOOKUP",
    "ASM DEG2", 
    "ASM DEG4", 
    "ASM ARTANH", 
    "ASM LOOKUP"
    );
    puts(seperator);
    
    for (size_t i = 0; i < 5; i++)
    {
        struct Handler file;
        file.data = NULL;
        file.len = 0;
        file.status = -1;

        char buffer[50];
        snprintf(buffer, sizeof(buffer), "%s%s", dir, files_performance[i]);
        file = handle_file(buffer);

        if (file.data != NULL)
        {

            printf("%-23s| ", files_performance[i]);
            double time = 0;

            time = time_calc(it, file.len, file.data, log2f, entropy_scalar);
            printf("%-16f| ", time);

            time = time_calc(it, file.len, file.data, log2_deg2_scalar, entropy_scalar);
            printf("%-16f| ", time);

            time = time_calc(it, file.len, file.data, log2_deg4_scalar, entropy_scalar);
            printf("%-16f| ", time);

            time = time_calc(it, file.len, file.data, log2_artanh_scalar, entropy_scalar);
            printf("%-16f| ", time);

            time = time_calc(it, file.len, file.data, log2_lookup_scalar, entropy_scalar);
            printf("%-16f| ", time);

            time = time_calc(it, file.len, file.data, log2_deg2_scalar_asm, entropy_scalar_asm);
            printf("%-16f| ", time);

            time = time_calc(it, file.len, file.data, log2_deg4_scalar_asm, entropy_scalar_asm);
            printf("%-16f| ", time);

            time = time_calc(it, file.len, file.data, log2_artanh_scalar_asm, entropy_scalar_asm);
            printf("%-16f| ", time);

            time = time_calc(it, file.len, file.data, log2_lookup_scalar_asm, entropy_scalar_asm);
            printf("%-16f", time);

            printf("\n");
            free(file.data);
        }
    }

    puts(seperator);

    puts(seperator);
    printf("%-23s| %-16s| %-16s| %-16s| %-16s| %-16s| %-16s| %-16s| %-16s| %-16s\n",
    "FILE",
    "SIMD GLIBC", 
    "SIMD DEG2", 
    "SIMD DEG4", 
    "SIMD ARTANH", 
    "SIMD LOOKUP",
    "ASM SIMD DEG2", 
    "ASM SIMD DEG4", 
    "ASM SIMD ARTANH", 
    "ASM SIMD LOOKUP"
    );
    puts(seperator);


  for (size_t i = 0; i < 5; i++)
    {
        struct Handler file;
        file.data = NULL;
        file.len = 0;
        file.status = -1;

        char buffer[50];
        snprintf(buffer, sizeof(buffer), "%s%s", dir, files_performance[i]);
        file = handle_file(buffer);

        if (file.data != NULL)
        {

            printf("%-23s| ", files_accuracy[i]);
            double time = 0;

            time = time_calc_simd(it, file.len, file.data, log2_glibc_simd, entropy_simd);
            printf("%-16f| ", time);

            time = time_calc_simd(it, file.len, file.data, log2_deg2_simd, entropy_simd);
            printf("%-16f| ", time);

            time = time_calc_simd(it, file.len, file.data, log2_deg4_simd, entropy_simd);
            printf("%-16f| ", time);

            time = time_calc_simd(it, file.len, file.data, log2_artanh_simd, entropy_simd);
            printf("%-16f| ", time);

            time = time_calc_simd(it, file.len, file.data, log2_lookup_simd, entropy_simd);
            printf("%-16f| ", time);

            time = time_calc_simd(it, file.len, file.data, log2_deg2_simd_asm, entropy_simd_asm);
            printf("%-16f| ", time);

            time = time_calc_simd(it, file.len, file.data, log2_deg4_simd_asm, entropy_simd_asm);
            printf("%-16f| ", time);

            time = time_calc_simd(it, file.len, file.data, log2_artanh_simd_asm, entropy_simd_asm);
            printf("%-16f| ", time);

            time = time_calc_simd(it, file.len, file.data, log2_lookup_simd_asm, entropy_simd_asm);
            printf("%-16f", time);


            printf("\n");
            free(file.data);
        }
    }

    puts(seperator);

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

        char buffer[50];
        snprintf(buffer, sizeof(buffer), "%s%s", dir, files_accuracy[i]);
        file = handle_file(buffer);

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


    puts("\n");
    puts("Executing tests for Accuracy...");
    puts(seperator);
    printf("%-23s| %-16s| %-16s| %-16s| %-16s| %-16s| %-16s| %-16s| %-16s| %-16s\n",
    "",
    "LOG2F", 
    "DEG2", 
    "DEG4", 
    "ARTANH", 
    "LOOKUP",
    "ASM DEG2", 
    "ASM DEG4", 
    "ASM ARTANH", 
    "ASM LOOKUP"
    );
    puts(seperator);

    if (success != 0)
    {
        double abs_mistake = 0;
        printf("%-23s| ", "Absolute Mistake");

        abs_mistake = array[0] / success;
        printf("%-16f| ", abs_mistake);

        abs_mistake = array[1] / success;
        printf("%-16f| ", abs_mistake);

        abs_mistake = array[2] / success;
        printf("%-16f| ", abs_mistake);

        abs_mistake = array[3] / success;
        printf("%-16f| ", abs_mistake);

        abs_mistake = array[4] / success;
        printf("%-16f| ", abs_mistake);

        abs_mistake = array[5] / success;
        printf("%-16f| ", abs_mistake);

        abs_mistake = array[6] / success;
        printf("%-16f| ", abs_mistake);

        abs_mistake = array[7] / success;
        printf("%-16f| ", abs_mistake);

        abs_mistake = array[8] / success;
        printf("%-16f\n", abs_mistake);

    }
    else
    {
        printf("No entropy was calculated make sure that test files exist. \n");
    }

    puts(seperator);

    puts(seperator);
    printf("%-23s| %-16s| %-16s| %-16s| %-16s| %-16s| %-16s| %-16s| %-16s| %-16s\n",
    "",
    "SIMD GLIBC", 
    "SIMD DEG2", 
    "SIMD DEG4", 
    "SIMD ARTANH", 
    "SIMD LOOKUP",
    "ASM SIMD DEG2", 
    "ASM SIMD DEG4", 
    "ASM SIMD ARTANH", 
    "ASM SIMD LOOKUP"
    );
    puts(seperator);

    if (success != 0)
    {
        double abs_mistake = 0;
        printf("%-23s| ", "Absolute Mistake");

        abs_mistake = array[9] / success;
        printf("%-16f| ", abs_mistake);

        abs_mistake = array[10] / success;
        printf("%-16f| ", abs_mistake);

        abs_mistake = array[11] / success;
        printf("%-16f| ", abs_mistake);

        abs_mistake = array[12] / success;
        printf("%-16f| ", abs_mistake);

        abs_mistake = array[13] / success;
        printf("%-16f| ", abs_mistake);

        abs_mistake = array[14] / success;
        printf("%-16f| ", abs_mistake);

        abs_mistake = array[15] / success;
        printf("%-16f| ", abs_mistake);

        abs_mistake = array[16] / success;
        printf("%-16f| ", abs_mistake);

        abs_mistake = array[17] / success;
        printf("%-16f\n", abs_mistake);

    }
    else
    {
        printf("No entropy was calculated make sure that test files exist. \n");
    }

    puts(seperator);
}
