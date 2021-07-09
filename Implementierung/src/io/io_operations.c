#include "io_operations.h"

// This function determines the size of inputs in the given file name.
static size_t size_file(const char *file_name) {
    FILE *input_file;
    input_file = fopen(file_name, "r");

    if (input_file == NULL) {
        printf("Error occurred while trying to open the input file: %s.\n", file_name);
        return 0;
    }

    size_t len = 0;
    float tmp = 0;

    // First reading the file until an error occurs or until reach the end of line.
	while (fscanf(input_file, "%f", &tmp) == 1)
	{
		++len;
	}
	fclose(input_file);
	return len;
}

static float *read_file(size_t len, const char *file_name) {
    // allocate enough spaces to store every input.
    size_t align = len + (4 - len % 4) % 4;
    float *inputs = aligned_alloc(16, align * sizeof(float));

    if (inputs == NULL) {
        printf("Could not allocated enough memory to store every input.\n");
        printf("Number of elements to store: %zu.\n", len);
        return NULL;
    }

    FILE *input_file;
    input_file = fopen(file_name, "r");

    if (input_file == NULL) {
        printf("Error occurred while trying to open the input file: %s.\n", file_name);
        free(inputs);
        return NULL;
    }

    // take inputs from the file and store it in inputs.
	for (size_t i = 0; i < len; ++i)
	{
		if (!fscanf(input_file, "%f", &inputs[i]))
		{
			free(inputs);
			return NULL;
		}
	}
	for (size_t i = len; i < align; ++i)
	{
		inputs[i] = 0.0f;
	}

	// free resources
	fclose(input_file);

	return inputs;
}

struct Handler handle_file(const char *file_name) {
    struct Handler handler;
    handler.len = 0;
    handler.data = NULL;
    handler.status = -1;
    size_t len = size_file(file_name);
    if (len == 0) {
        printf("No numbers are read from the file %s.\n", file_name);
        return handler;
    }
    handler.data = read_file(len, file_name);
    if (handler.data == NULL) {
        return handler;
    }
    handler.len = len;
    handler.status = 0;
    return handler;
}

float calculate_entropy(size_t n, const float *data, enum Language lan, enum Mode mode, enum Implementation impl)
{
    switch (lan)
    {
    case C:
        switch (mode)
        {
        case SCALAR:
            switch (impl)
            {
            case DEG2:
                return entropy_scalar(n, data, log2_deg2_scalar);
            case DEG4:
                return entropy_scalar(n, data, log2_deg4_scalar);
            case ARTANH:
                return entropy_scalar(n, data, log2_artanh_scalar);
            case LOOKUP:
                return entropy_scalar(n, data, log2_lookup_scalar);
            case LOG2F:
                return entropy_scalar(n, data, log2f);
            }
            break;
        case SIMD:
            switch (impl)
            {
            case DEG2:
                return entropy_simd(n, data, log2_deg2_simd);
            case DEG4:
                return entropy_simd(n, data, log2_deg4_simd);
            case ARTANH:
                return entropy_simd(n, data, log2_artanh_simd);
            case LOOKUP:
                return entropy_simd(n, data, log2_lookup_simd);
            case LOG2F:
                return entropy_simd(n, data, log2_glibc_simd);
            default:
                break;
            }
            break;
        }
        break;
    case ASM:
        switch (mode)
        {
        case SCALAR:
            switch (impl)
            {
            case DEG2:
                return entropy_scalar_asm(n, data, log2_deg2_scalar_asm);
            case DEG4:
                return entropy_scalar_asm(n, data, log2_deg4_scalar_asm);
            case ARTANH:
                return entropy_scalar_asm(n, data, log2_artanh_scalar_asm);
            case LOOKUP:
                return entropy_scalar_asm(n, data, log2_lookup_scalar_asm);
            default:
                break;
            }
            break;
        case SIMD:
            switch (impl)
            {
            case DEG2:
                return entropy_simd_asm(n, data, log2_deg2_simd_asm);
            case DEG4:
                return entropy_simd_asm(n, data, log2_deg4_simd_asm);
            case ARTANH:
                return entropy_simd_asm(n, data, log2_artanh_simd_asm);
            case LOOKUP:
                return entropy_simd_asm(n, data, log2_lookup_simd_asm);
            default:
                break;
            }
            break;
        }
        break;
    }

	// If no such combination exists
    return -1.0f;
}

void print_entropy(enum Language lan, enum Mode mode, enum Implementation impl, float entropy)
{
    char *lans = lan == C ? "C" : "ASM";
    char *modes = mode == SCALAR ? "scalar" : "simd";
    char *impls = "";

    switch (impl)
    {
    case DEG2:
        impls = "DEG2";
        break;
    case DEG4:
        impls = "DEG4";
        break;
    case ARTANH:
        impls = "ARTANH";
        break;
    case LOOKUP:
        impls = "LOOKUP";
        break;
    case LOG2F:
        impls = "LOG2F";
        break;
    }

	// Alignment to make it look better in console
    int32_t len = 20 - (strlen(lans) + strlen(modes) + strlen(impls));

    printf("%s/%s/%s Entropy:%*s%f\n", lans, modes, impls, len, "", entropy);
}

void print_mistake(float entropy, double precise_entropy)
{
    double abs_mistake = fabs(precise_entropy - entropy);
    printf("Absolute Mistake:%*s%f\n", 14, "", abs_mistake);

    // if (precise_entropy != 0)
    // {
    //     printf("Relative Mistake:%*s%f\n", 14, "", abs_mistake / precise_entropy);
    // }
    // else
    // {
    //     printf("No relative mistake is calculated because precise entropy is: %f \n",precise_entropy);
    // }
}

void evaluate_args(size_t n, const float *data, enum Language lan, enum Mode mode,
                   enum Implementation impl, double precise_entropy, bool accuracy, bool time, size_t iterations)
{
    float entropy = calculate_entropy(n, data, lan, mode, impl);

    print_entropy(lan, mode, impl, entropy);

    if (accuracy)
    {
        print_mistake(entropy, precise_entropy);
    }

    if (time)
    {
        struct timespec start, end;
        clock_gettime(CLOCK_MONOTONIC, &start);

        // Need more than 1 iterations to measure time
        for (size_t i = 0; i < iterations; ++i)
        {
            calculate_entropy(n, data, lan, mode, impl);
        }

        clock_gettime(CLOCK_MONOTONIC, &end);

        double time_secs = (end.tv_sec - start.tv_sec) + 1e-9 * (end.tv_nsec - start.tv_nsec);
        printf("Calculation took: %*s%f seconds\n", 13, "", time_secs);
    }

    if (time || accuracy)
        printf("\n");
}

void run_full(size_t n, const float *data, double precise_entropy, bool accuracy, bool time, size_t iterations)
{
    printf("Length is:%*s %zu \n\n", 20, "", n);

    evaluate_args(n, data, C, SCALAR, LOG2F, precise_entropy, accuracy, time, iterations);
    evaluate_args(n, data, C, SCALAR, DEG2, precise_entropy, accuracy, time, iterations);
    evaluate_args(n, data, C, SCALAR, DEG4, precise_entropy, accuracy, time, iterations);
    evaluate_args(n, data, C, SCALAR, ARTANH, precise_entropy, accuracy, time, iterations);
    evaluate_args(n, data, C, SCALAR, LOOKUP, precise_entropy, accuracy, time, iterations);

    evaluate_args(n, data, C, SIMD, LOG2F, precise_entropy, accuracy, time, iterations);
    evaluate_args(n, data, C, SIMD, DEG2, precise_entropy, accuracy, time, iterations);
    evaluate_args(n, data, C, SIMD, DEG4, precise_entropy, accuracy, time, iterations);
    evaluate_args(n, data, C, SIMD, ARTANH, precise_entropy, accuracy, time, iterations);
    evaluate_args(n, data, C, SIMD, LOOKUP, precise_entropy, accuracy, time, iterations);

    evaluate_args(n, data, ASM, SCALAR, DEG2, precise_entropy, accuracy, time, iterations);
    evaluate_args(n, data, ASM, SCALAR, DEG4, precise_entropy, accuracy, time, iterations);
    evaluate_args(n, data, ASM, SCALAR, ARTANH, precise_entropy, accuracy, time, iterations);
    evaluate_args(n, data, ASM, SCALAR, LOOKUP, precise_entropy, accuracy, time, iterations);

    evaluate_args(n, data, ASM, SIMD, DEG2, precise_entropy, accuracy, time, iterations);
    evaluate_args(n, data, ASM, SIMD, DEG4, precise_entropy, accuracy, time, iterations);
    evaluate_args(n, data, ASM, SIMD, ARTANH, precise_entropy, accuracy, time, iterations);
    evaluate_args(n, data, ASM, SIMD, LOOKUP, precise_entropy, accuracy, time, iterations);
}

void print_usage()
{
    printf("Usage: entropy [options] file\n"
           "\t-l, --language => implementation language c|asm.\n"
           "\t-m, --mode => run mode scalar|simd.\n"
           "\t-i, --implementation => deg2|deg4|artanh|lookup.\n"
           "\t-t, --time => calculate time that program takes optional [iterations].\n"
           "\t-a, --accuracy => difference with double precision scalar entropy.\n"
           "\t-r, --random => run with random data.\n"
           "\t-g, --generator => selects random generator rand|urandom. Default is rand.\n"
           "\t-u, --uniform => makes the random distribution uniform.\n"
           "\t-f, --full => run tests for the data with all possible configurations.\n"
           "\t-b, --benchmark => run benchmarks.\n"
           "\t-h, --help\n");
}
