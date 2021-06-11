#include "entropy.h"

int compare_function(const void* a, const void* b)
{
    int x = *(const int*)a;
    int y = *(const int*)b;
    return (x > y) - (x < y);
}

float entropy_c_rand(size_t len)
{
    if (len == 0)
    {
        printf("Length must be a positive number.");
        return -1;
    }
    srand(time(NULL));

    int* in_array = malloc(len * sizeof(int));
    float* prob_array = malloc(len * sizeof(float));

    if (in_array == NULL || prob_array == NULL)
    {
        printf("Could not allocate enough space for given length: %zu. \n", len);
        return -1;
    }

    for (size_t i = 0; i < len; ++i)
    {
        in_array[i] = rand();
    }


    qsort(in_array, len, sizeof(int), compare_function);

    size_t index = 0;
    size_t freq = 1;

    for (size_t i = 1; i < len; ++i)
    {
        if (unlikely(in_array[i] == in_array[i - 1]))
        {
            ++freq;
        }
        else
        {
            prob_array[index++] = ((float)freq) / len;
            freq = 1;
        }
    }

    prob_array[index++] = ((float)freq) / len;

    float entropy = scalar_entropy(index, prob_array, log2approx_deg4);

    free(in_array);
    free(prob_array);
    return entropy;
}

float entropy_c_urandom(size_t len)
{
    if (len == 0)
    {
        printf("Length must be a positive number.");
        return -1;
    }
    FILE* input_file;
    input_file = fopen("/dev/urandom", "r");
    if (input_file == NULL)
    {
        printf("Error occurred while trying to open  /dev/urandom");
        return -1;
    }

    int* in_array = malloc(len * sizeof(int));
    float* prob_array = malloc(len * sizeof(float));

    if (in_array == NULL || prob_array == NULL)
    {
        printf("Could not allocate enough space for given length: %zu. \n", len);
        fclose(input_file);
        return -1;
    }

    if(!fread(in_array, len, 4, input_file)) {
        printf("Nothing read!\n");
        fclose(input_file);
        free(in_array);
        free(prob_array);
        return -1;
    }

    qsort(in_array, len, sizeof(int), compare_function);

    size_t index = 0;
    size_t freq = 1;

    for (size_t i = 1; i < len; ++i)
    {
        if (unlikely(in_array[i] == in_array[i - 1]))
        {
            ++freq;
        }
        else
        {
            prob_array[index++] = ((float)freq) / len;
            freq = 1;
        }
    }

    prob_array[index++] = ((float)freq) / len;

    float entropy = scalar_entropy(index, prob_array, log2approx_arctanh);

    fclose(input_file);
    free(in_array);
    free(prob_array);
    return entropy;
}