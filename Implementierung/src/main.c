#include <stdlib.h>
#include <getopt.h>
#include <time.h>
#include <stdbool.h>
#include <string.h>
#include <math.h>

#include "entropy.h"
#include "io_operations.h"

const char *optstring = ":l:m:i:r:tah";

enum Language { C, ASM };
enum Mode { SCALAR, SIMD };
enum Implementation { DEG2, DEG4, ARTANH, LOOKUP };
/*
    -l, --language => implementation language c|asm
	-m, --mode => run mode scalar|simd
    -i, --implementation => deg2|deg4|artanh|lookup
	-t, --time => calculate time that program takes
	-a, --accuracy => difference with double precision scalar entropy
    -r, --random => test with random data
	-h, --help 
*/

float evaluate_entropy(size_t n, float* data, enum Language lan, enum Mode mode, enum Implementation impl){
    switch(lan){
    case C:
        switch(mode){
        case SCALAR:
            switch (impl) {
            case DEG2:
                return scalar_entropy(n, data, log2approx_deg2);
            case DEG4:
                return scalar_entropy(n, data, log2approx_deg4);
            case ARTANH:
                return scalar_entropy(n, data, log2approx_arctanh);
            case LOOKUP:
                return scalar_entropy(n, data, log2_lookup);
            }
        case SIMD:
            switch (impl) {
            case DEG2:
                return simd_entropy(n, data, log2approx_deg2_simd);
            case DEG4:
                return simd_entropy(n, data, log2approx_deg4_simd);
            case ARTANH:
                return simd_entropy(n, data, log2approx_arctanh_simd);
            case LOOKUP:
                return simd_entropy(n, data, log2_lookup_simd);
            }
        }
    case ASM:
        switch(mode){
        case SCALAR:
            switch (impl) {
            case DEG2:
                return entropy_asm(n, data, log2approx_deg2_asm);
            case DEG4:
                return entropy_asm(n, data, log2approx_deg4_asm);
            case ARTANH:
                return entropy_asm(n, data, log2approx_arctanh_asm);
            case LOOKUP:
                return entropy_asm(n, data, log2_lookup_asm);
            }
        case SIMD:
            switch (impl) {
            case DEG2:
                return entropy_simd(n, data, log2approx_deg2_simd_asm);
            case DEG4:
                return entropy_simd(n, data, log2approx_deg4_simd_asm);
            case ARTANH:
                return entropy_simd(n, data, log2approx_arctanh_simd_asm);
            case LOOKUP:
                return entropy_simd(n, data, log2_lookup_simd_asm);
            }
        }
    }
}

void printEntropy(enum Language lan, enum Mode mode, enum Implementation impl, float entropy){
    char* lans = lan == C ? "C" : "ASM";
    char* modes = mode == SCALAR ? "scalar" : "simd";
    char* impls;
    switch(impl){
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
    }
    printf("%s/%s/%s Entropy:\t%f\n", lans, modes,impls, entropy);
}

void printMistake(float entropy, double preciseEntropy){
    double absMistake = fabs(preciseEntropy - entropy);
    printf("Precise Entropy:\t%f\n", preciseEntropy);
    printf("Absolute Mistake:\t%f\n", absMistake);
    printf("Relative Mistake:\t%f\n", absMistake/preciseEntropy);
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

	static struct option long_options[] = {
	    {"language", required_argument, 0, 'l'},
		{"mode", required_argument, 0, 'm'},
        {"implementation", required_argument, 0, 'i'},
		{"help", no_argument, 0, 'h'},
		{"accuracy", no_argument, 0, 'a'},
		{"time", no_argument, 0, 't'},
        {"random", required_argument, 0, 'r'},
		{0, 0, 0, 0}};

	int optindex = 0;

	enum Language lan = C;
	enum Mode mode = SCALAR;
	enum Implementation impl = DEG2;
	size_t randLen = 0;
	bool time = false;
	bool accuracy = false;
	// Fetching option arguments
	while ((opt = getopt_long(argc, argv, optstring, long_options, &optindex)) != -1)
	{

		switch (opt)
		{
		case 'l':
		    if(strcmp("c", optarg) == 0)
		        lan = C;
		    else if(strcmp("asm", optarg) == 0)
		        lan = ASM;
		    else {
                printf("Wrong language option!\n");
                exit(EXIT_FAILURE);
		    }
			break;
        case 'm':
            if(strcmp("scalar", optarg) == 0)
                mode = SCALAR;
            else if(strcmp("simd", optarg) == 0)
                mode = SIMD;
            else {
                printf("Wrong mode option!\n");
                exit(EXIT_FAILURE);
            }
            break;
        case 'i':
            if(strcmp("deg2", optarg) == 0)
                impl = DEG2;
            else if(strcmp("deg4", optarg) == 0)
                impl = DEG4;
            else if(strcmp("artanh", optarg) == 0)
                impl = ARTANH;
            else if(strcmp("lookup", optarg) == 0)
                impl = LOOKUP;
            else {
                printf("Wrong implementation option!\n");
                exit(EXIT_FAILURE);
            }
            break;
        case 'r':
            randLen = atoi(optarg);
            break;
		case 't':
		    time = true;
            break;
			printf("time \n");
			struct timespec start, end;
			double sec;
			float entropy;
			char *file_name = "tests/a.txt";
			struct Handler handler;
			handler = handle_file(file_name);
			if (handler.status == 0)
			{
				size_t iterations = 100000000;
				clock_gettime(CLOCK_MONOTONIC, &start);
				for (size_t i = 0; i < iterations; ++i)
				{
					entropy = simd_entropy(handler.simd_len, handler.data, log2approx_deg4_simd_asm);
				}
				clock_gettime(CLOCK_MONOTONIC, &end);
				sec = end.tv_sec - start.tv_sec + 1e-9 * (end.tv_nsec - start.tv_nsec);
				printf("It took %f seconds to calculate entropy %zu times.\n", sec, iterations);
				printf("Entropy is: %f \n", entropy);
				free(handler.data);
			}

			break;
		case 'a':
		    accuracy = true;
			break;
		case 'h':
			printf("Usage: entropy [options] file\n"
                   "\t-l, --language => implementation language c|asm\n"
                   "\t-m, --mode => run mode scalar|simd\n"
                   "\t-i, --implementation => deg2|deg4|artanh|lookup\n"
                   "\t-t, --time => calculate time that program takes\n"
                   "\t-a, --accuracy => difference with double precision scalar entropy\n"
                   "\t-h, --help\n"
                   "\t-r, --random => run with random data\n"
            );
			return EXIT_SUCCESS;

		case ':':
			fprintf(stderr, "Missing argumant for option -%c\n", optopt);
			__attribute__((fallthrough)); // Let fallthrough

		default:
			fprintf(stderr, "USAGE HERE\n");
			exit(EXIT_FAILURE);
		}
	}

	struct Handler handler;
	handler = handle_file(argv[optind]);
	if(handler.status == -1 && randLen == 0)
	    exit(EXIT_FAILURE);

	if(randLen != 0){
	    printf("Calculating entropy of random data.\n");
	    handler.len = randLen;
	    handler.data = entropy_c_rand(randLen);
	}

	clock_t start;
	start = clock();
	float entropy =	evaluate_entropy(handler.len, handler.data, lan, mode, impl);
    double time_secs = (clock() - start)/(double)CLOCKS_PER_SEC;

    printEntropy(lan, mode, impl, entropy);
    if(time){
        printf("Calculation took: \t%f seconds\n", time_secs);
    }
    if(accuracy){
        float preciseEntropy = precise_entropy(handler.len, handler.data);
        printMistake(entropy, preciseEntropy);
    }

    free(handler.data);
	exit(EXIT_SUCCESS);
}
