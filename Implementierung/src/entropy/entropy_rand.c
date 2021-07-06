#include "entropy.h"

// Compare function for qsort
static int compare_function_int(const void *a, const void *b)
{
    int32_t x = *(const int32_t *)a;
    int32_t y = *(const int32_t *)b;
    return (x > y) - (x < y);
}

/*
    Creates a probability distribution with rand function.
    P(X = x) = numberof(x) / len;
*/
float *entropy_rand(size_t len)
{
    if (len == 0)
    {
        printf("Length must be a positive number.");
        return NULL;
    }
    srand(time(NULL));

    size_t align = len + (4 - len % 4) % 4;

    int32_t *in_array = malloc(len * sizeof(int32_t));

    if (in_array == NULL)
    {
        printf("Could not allocate enough space for given length: %zu. \n", len);
        return NULL;
    }

    float *prob_array = aligned_alloc(16, align * sizeof(float));

    if (prob_array == NULL)
    {
        printf("Could not allocate enough space for given length: %zu. \n", len);
        free(in_array);
        return NULL;
    }

    for (size_t i = 0; i < len; ++i)
    {
        in_array[i] = rand();
    }

    qsort(in_array, len, sizeof(int), compare_function_int);

    size_t index = 0;
    size_t freq = 1;

    // Counting the occurences of every number and updating probability array.
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

    // Fill the rest of them zero.
    for (size_t i = index; i < align; ++i)
    {
        prob_array[i] = 0.0f;
    }

    free(in_array);
    return prob_array;
}

/*
    Creates a probability distribution with /dev/urandom.
    P(X = x) = numberof(x) / len;
*/
float *entropy_urandom(size_t len)
{
    if (len == 0)
    {
        printf("Length must be a positive number.");
        return NULL;
    }

    FILE *input_file;
    input_file = fopen("/dev/urandom", "r");

    if (input_file == NULL)
    {
        printf("Error occurred while trying to open  /dev/urandom");
        return NULL;
    }

    size_t align = len + (4 - len % 4) % 4;

    int32_t *in_array = malloc(len * sizeof(int32_t));

    if (in_array == NULL)
    {
        printf("Could not allocate enough space for given length: %zu. \n", len);
        fclose(input_file);
        return NULL;
    }

    float *prob_array = aligned_alloc(16, align * sizeof(float));

    if (prob_array == NULL)
    {
        printf("Could not allocate enough space for given length: %zu. \n", len);
        fclose(input_file);
        free(in_array);
        return NULL;
    }

    if (!fread(in_array, len, 4, input_file))
    {
        printf("Nothing read!\n");
        fclose(input_file);
        free(in_array);
        free(prob_array);
        return NULL;
    }

    qsort(in_array, len, sizeof(int), compare_function_int);

    size_t index = 0;
    size_t freq = 1;

    // Counting the occurences of every number and updating probability array.
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

    // Fill the rest of them zero.
    for (size_t i = index; i < align; ++i)
    {
        prob_array[i] = 0.0f;
    }

    fclose(input_file);
    free(in_array);
    return prob_array;
}

/*
    Creates a probability distribution with rand function.
    P(X = x) = x/sum(array)
*/
float *entropy_rand_non_uniform(size_t len)
{
    if (len == 0)
    {
        printf("Length must be a positive number.");
        return NULL;
    }
    srand(time(NULL));

    size_t align = len + (4 - len % 4) % 4;
    float *prob_array = aligned_alloc(16, align * sizeof(float));
    if (prob_array == NULL)
    {
        printf("Could not allocate enough space for given length: %zu. \n", len);
        return NULL;
    }
    // using double for more precision.
    double sum = 0;
    for (size_t i = 0; i < len; ++i)
    {
        double x = (double)rand() / (double)RAND_MAX;
        sum += x;
        prob_array[i] = (float)x;
    }
    for (size_t i = len; i < align; ++i)
    {
        prob_array[i] = 0;
    }
    // This shouldnt happen because every number between 0 and 1.
    // But to be just safe.
    if (sum <= 0)
    {
        free(prob_array);
        printf("Overflow occurred please enter a lower length. \n");
        return NULL;
    }
    // Divide Every number by the sum to normalize.
    for (size_t i = 0; i < align; ++i)
    {
        prob_array[i] = (float)((double)prob_array[i] / sum);
    }

    return prob_array;
}

/*
    Creates a probability distribution with /dev/urandom.
    P(X = x) = x/sum(array)
*/
float *entropy_urandom_non_uniform(size_t len)
{
    if (len == 0)
    {
        printf("Length must be a positive number.");
        return NULL;
    }
    FILE *input_file;
    input_file = fopen("/dev/urandom", "r");
    if (input_file == NULL)
    {
        printf("Error occurred while trying to open  /dev/urandom");
        return NULL;
    }

    size_t align = len + (4 - len % 4) % 4;

    uint32_t *in_array = malloc(len * sizeof(uint32_t));

    if (in_array == NULL)
    {
        printf("Could not allocate enough space for given length: %zu. \n", len);
        fclose(input_file);
        return NULL;
    }

    float *prob_array = aligned_alloc(16, align * sizeof(float));

    if (prob_array == NULL)
    {
        printf("Could not allocate enough space for given length: %zu. \n", len);
        fclose(input_file);
        free(in_array);
        return NULL;
    }

    if (!fread(in_array, len, 4, input_file))
    {
        printf("Nothing read!\n");
        fclose(input_file);
        free(in_array);
        free(prob_array);
        return NULL;
    }
    // using double for more precision.
    double sum = 0;
    for (size_t i = 0; i < len; ++i)
    {
        prob_array[i] = (float)((double)in_array[i] / (double)RAND_MAX);
        sum += (double)prob_array[i];
    }

    for (size_t i = len; i < align; ++i)
    {
        prob_array[i] = 0.0f;
    }
    // This shouldnt happen because every number between 0 and 1.
    // But to be just safe.
    if (sum <= 0)
    {
        printf("Overflow occurred please enter a lower length. \n");
        fclose(input_file);
        free(in_array);
        free(prob_array);
        return NULL;
    }
    
    for (size_t i = 0; i < align; ++i)
    {
        prob_array[i] = (float)((double)prob_array[i] / sum);
    }

    fclose(input_file);
    free(in_array);
    return prob_array;
}
