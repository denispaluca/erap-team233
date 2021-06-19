#include <stdlib.h>
#include <getopt.h>
#include <time.h>
#include <stdbool.h>
#include <string.h>
#include <math.h>
#include <string.h>

#include "entropy.h"
#include "io_operations.h"

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
/*
    -l, --language => implementation language c|asm
	-m, --mode => run mode scalar|simd
    -i, --implementation => deg2|deg4|artanh|lookup
	-t, --time => calculate time that program takes [optional iterations]
	-a, --accuracy => difference with double precision scalar entropy
    -r, --random => test with random data
    -f, --full => run tests for the data with all possible configurations
	-h, --help 
*/

float evaluate_entropy(size_t n, float *data, enum Language lan, enum Mode mode, enum Implementation impl)
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
                return scalar_entropy(n, data, log2approx_deg2);
            case DEG4:
                return scalar_entropy(n, data, log2approx_deg4);
            case ARTANH:
                return scalar_entropy(n, data, log2approx_arctanh);
            case LOOKUP:
                return scalar_entropy(n, data, log2_lookup);
            case LOG2F:
                return scalar_entropy(n, data, log2f);
            }
            break;
        case SIMD:
            switch (impl)
            {
            case DEG2:
                return simd_entropy(n, data, log2approx_deg2_simd);
            case DEG4:
                return simd_entropy(n, data, log2approx_deg4_simd);
            case ARTANH:
                return simd_entropy(n, data, log2approx_arctanh_simd);
            case LOOKUP:
                return simd_entropy(n, data, log2_lookup_simd);
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
                return entropy_asm(n, data, log2approx_deg2_asm);
            case DEG4:
                return entropy_asm(n, data, log2approx_deg4_asm);
            case ARTANH:
                return entropy_asm(n, data, log2approx_arctanh_asm);
            case LOOKUP:
                return entropy_asm(n, data, log2_lookup_asm);
            default:
                break;
            }
            break;
        case SIMD:
            switch (impl)
            {
            case DEG2:
                return entropy_simd(n, data, log2approx_deg2_simd_asm);
            case DEG4:
                return entropy_simd(n, data, log2approx_deg4_simd_asm);
            case ARTANH:
                return entropy_simd(n, data, log2approx_arctanh_simd_asm);
            case LOOKUP:
                return entropy_simd(n, data, log2_lookup_simd_asm);
            default:
                break;
            }
            break;
        }
        break;
    }
    return -1.0f;
}

void printEntropy(enum Language lan, enum Mode mode, enum Implementation impl, float entropy)
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
    int len = 20 - (strlen(lans) + strlen(modes) + strlen(impls));
    printf("%s/%s/%s Entropy:%*s%f\n", lans, modes, impls, len, "", entropy);
}

void printMistake(float entropy, double preciseEntropy)
{
    double absMistake = fabs(preciseEntropy - entropy);
    printf("Absolute Mistake:%*s%f\n", 14, "", absMistake);
    if (preciseEntropy != 0)
    {
        printf("Relative Mistake:%*s%f\n", 14, "", absMistake / preciseEntropy);
    }
    // else
    // {
    //     printf("No relative mistake is calculated because precise entropy is: %f \n",preciseEntropy);
    // }
}

void evaluate_args(size_t n, float *data, enum Language lan, enum Mode mode,
                   enum Implementation impl, float preciseEntropy, bool accuracy, bool time, size_t iterations)
{
    float entropy = evaluate_entropy(n, data, lan, mode, impl);

    printEntropy(lan, mode, impl, entropy);

    if (accuracy)
    {
        printMistake(entropy, preciseEntropy);
    }

    if (time)
    {
        struct timespec start, end;
        clock_gettime(CLOCK_MONOTONIC, &start);

        // Need more than 1 iterations to measure time
        for (size_t i = 0; i < iterations; ++i)
        {
            evaluate_entropy(n, data, lan, mode, impl);
        }

        clock_gettime(CLOCK_MONOTONIC, &end);

        double time_secs = (end.tv_sec - start.tv_sec) + 1e-9 * (end.tv_nsec - start.tv_nsec);
        printf("Calculation took: %*s%f seconds\n", 13, "", time_secs);
    }
    
    if (time || accuracy)
        printf("\n");
}

void runFull(size_t n, float *data, float preciseEntropy, bool accuracy, bool time, size_t iterations)
{
    printf("Length is:%*s %zu \n\n", 20, "", n);
    evaluate_args(n, data, C, SCALAR, LOG2F, preciseEntropy, accuracy, time, iterations);

    evaluate_args(n, data, C, SCALAR, DEG2, preciseEntropy, accuracy, time, iterations);
    evaluate_args(n, data, C, SCALAR, DEG4, preciseEntropy, accuracy, time, iterations);
    evaluate_args(n, data, C, SCALAR, ARTANH, preciseEntropy, accuracy, time, iterations);
    evaluate_args(n, data, C, SCALAR, LOOKUP, preciseEntropy, accuracy, time, iterations);

    evaluate_args(n, data, C, SIMD, DEG2, preciseEntropy, accuracy, time, iterations);
    evaluate_args(n, data, C, SIMD, DEG4, preciseEntropy, accuracy, time, iterations);
    evaluate_args(n, data, C, SIMD, ARTANH, preciseEntropy, accuracy, time, iterations);
    evaluate_args(n, data, C, SIMD, LOOKUP, preciseEntropy, accuracy, time, iterations);

    evaluate_args(n, data, ASM, SCALAR, DEG2, preciseEntropy, accuracy, time, iterations);
    evaluate_args(n, data, ASM, SCALAR, DEG4, preciseEntropy, accuracy, time, iterations);
    evaluate_args(n, data, ASM, SCALAR, ARTANH, preciseEntropy, accuracy, time, iterations);
    evaluate_args(n, data, ASM, SCALAR, LOOKUP, preciseEntropy, accuracy, time, iterations);

    evaluate_args(n, data, ASM, SIMD, DEG2, preciseEntropy, accuracy, time, iterations);
    evaluate_args(n, data, ASM, SIMD, DEG4, preciseEntropy, accuracy, time, iterations);
    evaluate_args(n, data, ASM, SIMD, ARTANH, preciseEntropy, accuracy, time, iterations);
    evaluate_args(n, data, ASM, SIMD, LOOKUP, preciseEntropy, accuracy, time, iterations);
}

void printUsage() {
    printf("Usage: entropy [options] file\n"
                   "\t-l, --language => implementation language c|asm\n"
                   "\t-m, --mode => run mode scalar|simd\n"
                   "\t-i, --implementation => deg2|deg4|artanh|lookup\n"
                   "\t-t, --time => calculate time that program takes optional [iterations]\n"
                   "\t-a, --accuracy => difference with double precision scalar entropy\n"
                   "\t-r, --random => run with random data\n"
                   "\t-f, --full => run tests for the data with all possible configurations\n"
                   "\t-h, --help\n");
}

int main(int argc, char *argv[])
{

    // optind -> index of next element to be processed in argv
    // nextchar -> if finds option character, returns it

    // no more option character, getopt returns -1
    // Then optind is the index in argv of the first argv-element that is not an option.

    // optstring is a string containing the legitimate option characters.
    // If such a character is followed by a colon, the option requires an argument, so getopt() places a pointer to the
    // following text in the same argv-element, or the text of the following argv-element, in optarg.
    // otherwise optarg is set to zero.

    //If an option was successfully found, then getopt() returns the option character.  If all command-line options have been parsed,
    // then getopt() returns -1.  If getopt() encounters an option character that was not in optstring, then '?' is returned.

    // If the first character of optstring is '-', then each nonoption argv-element is handled as if it were the argument
    // of an option with character code 1.

    // Long option names may be abbreviated if the abbreviation is unique or is an exact match for some defined option.
    // The last element of the array has to be filled with zeros.

    int opt;

    const char *optstring = ":-l:m:i:r:t::ahf";

    static struct option long_options[] = {
        {"language", required_argument, 0, 'l'},
        {"mode", required_argument, 0, 'm'},
        {"implementation", required_argument, 0, 'i'},
        {"help", no_argument, 0, 'h'},
        {"accuracy", no_argument, 0, 'a'},
        {"time", optional_argument, 0, 't'},
        {"random", required_argument, 0, 'r'},
        {"full", no_argument, 0, 'f'},
        {0, 0, 0, 0}};

    int optindex = 0;

    enum Language lan = C;
    enum Mode mode = SCALAR;
    enum Implementation impl = DEG4;
    size_t randLen = 0;
    bool time = false;
    size_t iterations = 1000;
    bool accuracy = false;
    bool full = false;

    // Fetching option arguments
    while ((opt = getopt_long(argc, argv, optstring, long_options, &optindex)) != -1)
    {

        switch (opt)
        {
        case 'l':
            if (strcmp("c", optarg) == 0)
                lan = C;
            else if (strcmp("asm", optarg) == 0)
                lan = ASM;
            else
            {
                printf("Wrong language option!\n");
                exit(EXIT_FAILURE);
            }
            break;
        case 'm':
            if (strcmp("scalar", optarg) == 0)
                mode = SCALAR;
            else if (strcmp("simd", optarg) == 0)
                mode = SIMD;
            else
            {
                printf("Wrong mode option!\n");
                exit(EXIT_FAILURE);
            }
            break;
        case 'i':
            if (strcmp("deg2", optarg) == 0)
                impl = DEG2;
            else if (strcmp("deg4", optarg) == 0)
                impl = DEG4;
            else if (strcmp("artanh", optarg) == 0)
                impl = ARTANH;
            else if (strcmp("lookup", optarg) == 0)
                impl = LOOKUP;
            else
            {
                printf("Wrong implementation option!\n");
                exit(EXIT_FAILURE);
            }
            break;
        case 'r':
            randLen = atoi(optarg);
            break;
        case 't':
            time = true;
            if (optarg != 0) {
                iterations = atoi(optarg);
            }
            break;
        case 'a':
            accuracy = true;
            break;
        case 'f':
            full = true;
            break;
        case 'h':
            printUsage();
            return EXIT_SUCCESS;

        case ':':
            fprintf(stderr, "Missing argumant for option -%c\n", optopt);
            __attribute__((fallthrough)); // Let fallthrough

        default:
            printUsage();
            exit(EXIT_FAILURE);
        }
    }

    struct Handler handler;
    handler.data = NULL;
    handler.len = 0;
    handler.status = -1;

    if (argv[optind] != NULL){
        handler = handle_file(argv[optind]);
        printf("-----------------------------------------------------\n");
        printf("       Calculating entropy of %s.\n",argv[optind]);
        printf("-----------------------------------------------------\n\n");
    }

    if (handler.status == -1 && randLen == 0) {
        // TODO Calculating... text should not be displayed
        printf("Invalid Data Input! \n");
        exit(EXIT_FAILURE);
    }

    if (randLen != 0)
    {
        printf("-----------------------------------------------------\n");
        printf("       Calculating entropy of random data.\n");
        printf("-----------------------------------------------------\n");
        handler.len = randLen;
        handler.data = entropy_c_rand(randLen);
    }

    if (handler.data == NULL)
    {
        printf("Bruh you check your pointers!!\n");
        exit(EXIT_FAILURE);
    }

    double preciseEntropy = 0.0;
    if (accuracy)
    {
        preciseEntropy = precise_entropy(handler.len, handler.data);
        printf("Precise Entropy:%*s%f\n", 15, "", preciseEntropy);
    }
    if (full)
        runFull(handler.len, handler.data, preciseEntropy, accuracy, time, iterations);
    else
        evaluate_args(handler.len, handler.data, lan, mode, impl, preciseEntropy, accuracy, time, iterations);

    printf("-----------------------------------------------------\n");
    printf("                    FINISHED\n");
    printf("-----------------------------------------------------\n");
    free(handler.data);
    exit(EXIT_SUCCESS);
}
