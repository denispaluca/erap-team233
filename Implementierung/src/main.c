#include <stdlib.h>
#include <getopt.h>
#include <time.h>
#include <stdbool.h>
#include <string.h>
#include <math.h>
#include <errno.h>

#include "entropy/entropy.h"
#include "../tests/tests.h"

int main(int argc, char *argv[]) {

    /*
    source: https://man7.org/linux/man-pages/man3/getopt.3.html
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

    const char *optstring = ":-l:m:i:r:t::ahfg:ub";

    const struct option long_options[] = {
            {"language",       required_argument, 0, 'l'},
            {"mode",           required_argument, 0, 'm'},
            {"implementation", required_argument, 0, 'i'},
            {"help",           no_argument,       0, 'h'},
            {"accuracy",       no_argument,       0, 'a'},
            {"time",           optional_argument, 0, 't'},
            {"random",         required_argument, 0, 'r'},
            {"generator",      required_argument, 0, 'g'},
            {"uniform",        no_argument,       0, 'u'},
            {"benchmark",      no_argument,       0, 'b'},
            {"full",           no_argument,       0, 'f'},
            {0, 0,                                0, 0}};

    int32_t opt;
    int32_t optindex = 0;

    // Initializing with default values
    enum Language lan = ASM;
    enum Mode mode = SIMD;
    enum Implementation impl = DEG4;
    size_t rand_len = 0;
    bool time = false;
    size_t iterations = 1000;
    bool accuracy = false;
    bool full = false;
    bool generator = false;
    bool uniform = false;
    bool benchmark = false;

    // Fetching option arguments
    while ((opt = getopt_long(argc, argv, optstring, long_options, &optindex)) != -1) {

        switch (opt) {
            case 'l':
                if (strcmp("c", optarg) == 0) {
                    lan = C;
                } else if (strcmp("asm", optarg) == 0) {
                    lan = ASM;
                } else {
                    printf("Wrong language option!\n");
                    exit(EXIT_FAILURE);
                }
                break;
            case 'm':
                if (strcmp("scalar", optarg) == 0) {
                    mode = SCALAR;
                } else if (strcmp("simd", optarg) == 0) {
                    mode = SIMD;
                } else {
                    printf("Wrong mode option!\n");
                    exit(EXIT_FAILURE);
                }
                break;
            case 'i':
                if (strcmp("deg2", optarg) == 0) {
                    impl = DEG2;
                } else if (strcmp("deg4", optarg) == 0) {
                    impl = DEG4;
                } else if (strcmp("artanh", optarg) == 0) {
                    impl = ARTANH;
                } else if (strcmp("lookup", optarg) == 0) {
                    impl = LOOKUP;
                } else if (strcmp("log2f", optarg) == 0) {
                    impl = LOG2F;
                } else {
                    printf("Wrong implementation option!\n");
                    exit(EXIT_FAILURE);
                }
                break;
            case 'r':
                errno = 0;
                rand_len = strtoul(optarg, NULL, 10);
                if (errno == ERANGE) {
                    printf("Please enter a number after -r. For example -r 1000 \n");
                    exit(EXIT_FAILURE);
                }
                break;
            case 'g':
                if (strcmp("rand", optarg) == 0) {
                    generator = false;
                } else if (strcmp("urandom", optarg) == 0) {
                    generator = true;
                } else {
                    printf("Wrong generator mode! \n");
                    exit(EXIT_FAILURE);
                }
                break;
            case 'u':
                uniform = true;
                break;
            case 't':
                time = true;
                errno = 0;
                if (optarg != 0) {
                    iterations = strtoul(optarg, NULL, 10);
                }
                if (errno == ERANGE) {
                    printf("Please enter a number after -t. For example -t500 \n");
                    exit(EXIT_FAILURE);
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
                printf("Missing argument for option -%c\n", optopt);
                        __attribute__((fallthrough)); // Let fallthrough

            default:
                print_usage();
                exit(EXIT_FAILURE);
        }
    }

    // Run benchmarks if flag provided before anything else
    if (benchmark) {
        if (!(accuracy || time)) {
            printf("You need to specify at least -a or -t to run benchmarks.\n");
            exit(EXIT_FAILURE);
        }

        if (time) {
            test_performance(iterations);
        }

        if (accuracy) {
            test_accuracy();
        }

        // After benchmarks no need to execute anything else
        exit(EXIT_SUCCESS);
    }
    if ((impl == LOG2F && lan == ASM) && !full) {
        printf("ASM|LOG2f is not implemented yet, please choose another implementation \n");
        exit(EXIT_FAILURE);
    }
    struct Handler handler;
    handler.data = NULL;
    handler.len = 0;
    handler.status = -1;

    if (argv[optind] != NULL) {
        handler = handle_file(argv[optind]);

        if (handler.status == -1) {
            exit(EXIT_FAILURE);
        }

        printf("-----------------------------------------------------\n");
        printf("       Calculating entropy of %s.\n", argv[optind]);
        printf("-----------------------------------------------------\n\n");
    } else if (rand_len != 0) {
        handler.len = rand_len;
        if (uniform) {
            if (generator) {
                handler.data = entropy_rand(rand_len);
            } else {
                handler.data = entropy_urandom(rand_len);
            }
        } else {
            if (generator) {
                handler.data = entropy_rand_non_uniform(rand_len);
            } else {
                handler.data = entropy_urandom_non_uniform(rand_len);
            }
        }
        if (handler.data != NULL) {
            printf("-----------------------------------------------------\n");
            printf("       Calculating entropy of random data.\n");
            printf("-----------------------------------------------------\n");
        }
    }

    if (handler.data == NULL) {
        //printf("Error occured while reading/generating a file.\n");
        exit(EXIT_FAILURE);
    }

    double precise_entropy = 0.0;
    if (accuracy) {
        precise_entropy = entropy_precise(handler.len, handler.data);
        printf("Precise Entropy:%*s%f\n", 15, "", precise_entropy);
        // struct timespec start, end;
        // clock_gettime(CLOCK_MONOTONIC, &start);
        // clock_gettime(CLOCK_MONOTONIC, &end);
        // double time_secs = (end.tv_sec - start.tv_sec) + 1e-9 * (end.tv_nsec - start.tv_nsec);
        // printf("Calculation took: %*s%f seconds\n", 13, "", time_secs);
    }

    if (full) {
        run_full(handler.len, handler.data, precise_entropy, accuracy, time, iterations);
    } else {
        printf("\n");
        evaluate_args(handler.len, handler.data, lan, mode, impl, precise_entropy, accuracy, time, iterations);
    }

    printf("-----------------------------------------------------\n");
    printf("                    FINISHED\n");
    printf("-----------------------------------------------------\n");
    free(handler.data);
    exit(EXIT_SUCCESS);
}
