#include "../src/entropy.h"
#include <locale.h>
#pragma GCC diagnostic ignored "-Wformat-security"



double time_calc(size_t it, size_t n, const float* arr, float (* log2_func) (float), float (*entropy)(size_t, const float*, float (*) (float))) {

    struct timespec start, end;
    clock_gettime(CLOCK_MONOTONIC, &start);

    for (size_t i = 0; i < it; i++) {
        entropy(n, arr, log2_func);
    }

    clock_gettime(CLOCK_MONOTONIC, &end);

    double time_secs = (end.tv_sec - start.tv_sec) + 1e-9 * (end.tv_nsec - start.tv_nsec);
    return time_secs / ((double) it);
}

void print_out(double t) {

    char* str = malloc(50);

    if (str == NULL) return;

    sprintf(str, "%.16g ", t);
    
    for (size_t i = 0; i < 50; i++)
    {
        if (str[i] == 0) break;

        if (str[i] == '.')
        str[i] = ',';
    }

    printf(str);
    free(str);
}


int main() {
    size_t it = 1;
    size_t n = 10;


    char* files[] = {
        "tests/testfiles/data_10_000_non-uni",
        "tests/testfiles/data_100_000_non-uni",
        "tests/testfiles/data_1_000_000_non-uni",
        "tests/testfiles/data_1_000_000_uni"
    };

    for (size_t i = 0; i < 4; i++)
    {
        struct Handler file;
        file = handle_file(files[i]);

        //float* arr = entropy_c_rand_non_uniform(n);

        print_out(time_calc(it, file.len, file.data, log2approx_deg2, scalar_entropy));
        print_out(time_calc(it, file.len, file.data, log2approx_deg4, scalar_entropy));
        print_out(time_calc(it, file.len, file.data, log2approx_arctanh, scalar_entropy));
        print_out(time_calc(it, file.len, file.data, log2_lookup, scalar_entropy));
        // print_out(time_calc(it, n, arr, log2_lookup_simd, simd_entropy));
        // print_out(time_calc(it, n, arr, log2_glibc_simd, simd_entropy));

        printf("\n");
        free(file.data);
    }
    


}