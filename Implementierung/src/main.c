#include <stdlib.h>
#include <getopt.h>
#include <time.h>
#include <stdbool.h>
#include <string.h>
#include <math.h>
#include <string.h>

#include "entropy/entropy.h"
#include "io/io_operations.h"
#include "../tests/tests.h"

enum Language
{
    C,
    ASM
};
enum Mode
{
    SCALAR,
    SIMD
};
enum Implementation
{
    DEG2,
    DEG4,
    ARTANH,
    LOOKUP,
    LOG2F
};

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
    int32_t len = 20 - (strlen(lans) + strlen(modes) + strlen(impls));
    printf("%s/%s/%s Entropy:%*s%f\n", lans, modes, impls, len, "", entropy);
}

void print_mistake(float entropy, double precise_entropy)
{
    double abs_mistake = fabs(precise_entropy - entropy);
    printf("Absolute Mistake:%*s%f\n", 14, "", abs_mistake);
    if (precise_entropy != 0)
    {
        printf("Relative Mistake:%*s%f\n", 14, "", abs_mistake / precise_entropy);
    }
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

int main(int argc, char *argv[])
{

    /*
    optind -> index of next element to be processed in argv
    nextchar -> if finds option character, returns it

    no more option character, getopt returns -1
    Then optind is the index in argv of the first argv-element that is not an option.

    optstring is a string containing the legitimate option characters.
    If such a character is followed by a colon, the option requires an argument, so getopt() places a pointer to the
    following text in the same argv-element, or the text of the following argv-element, in optarg.
    otherwise optarg is set to zero.

    If an option was successfully found, then getopt() returns the option character.  If all command-line options have been parsed,
    then getopt() returns -1.  If getopt() encounters an option character that was not in optstring, then '?' is returned.

    If the first character of optstring is '-', then each nonoption argv-element is handled as if it were the argument
    of an option with character code 1.

    Long option names may be abbreviated if the abbreviation is unique or is an exact match for some defined option.
    The last element of the array has to be filled with zeros.
    */

    int32_t opt;

    const char *optstring = ":-l:m:i:r:t::ahfg:ub";

    const struct option long_options[] = {
        {"language", required_argument, 0, 'l'},
        {"mode", required_argument, 0, 'm'},
        {"implementation", required_argument, 0, 'i'},
        {"help", no_argument, 0, 'h'},
        {"accuracy", no_argument, 0, 'a'},
        {"time", optional_argument, 0, 't'},
        {"random", required_argument, 0, 'r'},
        {"generator", required_argument, 0, 'g'},
        {"uniform", no_argument, 0, 'u'},
        {"benchmark", no_argument, 0, 'b'},
        {"full", no_argument, 0, 'f'},
        {0, 0, 0, 0}
    };

    int32_t optindex = 0;

    enum Language lan = ASM;
    enum Mode mode = SIMD;
    enum Implementation impl = DEG4;
    size_t randLen = 0;
    bool time = false;
    size_t iterations = 1000;
    bool accuracy = false;
    bool full = false;
    bool generator = false;
    bool uniform = false;
    bool benchmark = false;

    // Fetching option arguments
    while ((opt = getopt_long(argc, argv, optstring, long_options, &optindex)) != -1)
    {

        switch (opt)
        {
        case 'l':
            if (strcmp("c", optarg) == 0)
            {
                lan = C;
            }
            else if (strcmp("asm", optarg) == 0)
            {
                lan = ASM;
            }
            else
            {
                printf("Wrong language option!\n");
                exit(EXIT_FAILURE);
            }
            break;
        case 'm':
            if (strcmp("scalar", optarg) == 0)
            {
                mode = SCALAR;
            }
            else if (strcmp("simd", optarg) == 0)
            {
                mode = SIMD;
            }
            else
            {
                printf("Wrong mode option!\n");
                exit(EXIT_FAILURE);
            }
            break;
        case 'i':
            if (strcmp("deg2", optarg) == 0)
            {
                impl = DEG2;
            }
            else if (strcmp("deg4", optarg) == 0)
            {
                impl = DEG4;
            }
            else if (strcmp("artanh", optarg) == 0)
            {
                impl = ARTANH;
            }
            else if (strcmp("lookup", optarg) == 0)
            {
                impl = LOOKUP;
            }
            else
            {
                printf("Wrong implementation option!\n");
                exit(EXIT_FAILURE);
            }
            break;
        case 'r':
            randLen = atoi(optarg);
            break;
        case 'g':
            if (strcmp("rand", optarg) == 0)
            {
                generator = false;
            }
            else if (strcmp("urandom", optarg) == 0)
            {
                generator = true;
            }
            else
            {
                printf("Wrong generator mode! \n");
                exit(EXIT_FAILURE);
            }
            break;
        case 'u':
            uniform = true;
            break;
        case 't':
            time = true;
            if (optarg != 0)
            {
                iterations = atoi(optarg);
            }
            break;
        case 'a':
            accuracy = true;
            break;
        case 'f':
            full = true;
            break;
        case 'b':
            benchmark = true;
            break;
        case 'h':
            print_usage();
            return EXIT_SUCCESS;

        case ':':
            fprintf(stderr, "Missing argumant for option -%c\n", optopt);
            __attribute__((fallthrough)); // Let fallthrough

        default:
            print_usage();
            exit(EXIT_FAILURE);
        }
    }


    // Run benchmarks if flag provided before anything else
    if (benchmark) 
    {
        if (!(accuracy || time))
        {
            printf("You need to specify at least -a or -t to run benchmarks.\n");
            exit(EXIT_FAILURE);
        }

        if (time)
        {
            test_performance(iterations);
        }

        if (accuracy) 
        {
            test_accuracy();
        }

        exit(EXIT_SUCCESS);
    }

    struct Handler handler;
    handler.data = NULL;
    handler.len = 0;
    handler.status = -1;

    if (argv[optind] != NULL)
    {
        handler = handle_file(argv[optind]);

        if (handler.status == -1)
        {
            // TODO Calculating... text should not be displayed
            exit(EXIT_FAILURE);
        }

        printf("-----------------------------------------------------\n");
        printf("       Calculating entropy of %s.\n", argv[optind]);
        printf("-----------------------------------------------------\n\n");
    }

    if (argv[optind] == NULL && randLen != 0)
    {
        printf("-----------------------------------------------------\n");
        printf("       Calculating entropy of random data.\n");
        printf("-----------------------------------------------------\n");
        handler.len = randLen;
        if (uniform)
        {
            if (generator)
            {
                handler.data = entropy_rand(randLen);
            }
            else
            {
                handler.data = entropy_urandom(randLen);
            }
        }
        else
        {
            if (generator)
            {
                handler.data = entropy_rand_non_uniform(randLen);
            }
            else
            {
                handler.data = entropy_urandom_non_uniform(randLen);
            }
        }
    }

    if (handler.data == NULL)
    {
        printf("Error occured while reading/generating a file.\n");
        exit(EXIT_FAILURE);
    }

    double precise_entropy = 0.0;
    if (accuracy)
    {
        struct timespec start, end;
        clock_gettime(CLOCK_MONOTONIC, &start);
        precise_entropy = entropy_precise(handler.len, handler.data);
        clock_gettime(CLOCK_MONOTONIC, &end);
        double time_secs = (end.tv_sec - start.tv_sec) + 1e-9 * (end.tv_nsec - start.tv_nsec);
        printf("Precise Entropy:%*s%f\n", 15, "", precise_entropy);
        printf("Calculation took: %*s%f seconds\n", 13, "", time_secs);
    }
    if (full)
    {
        run_full(handler.len, handler.data, precise_entropy, accuracy, time, iterations);
    }
    else
    {
        printf("\n");
        evaluate_args(handler.len, handler.data, lan, mode, impl, precise_entropy, accuracy, time, iterations);
    }

    printf("-----------------------------------------------------\n");
    printf("                    FINISHED\n");
    printf("-----------------------------------------------------\n");
    free(handler.data);
    exit(EXIT_SUCCESS);
}
