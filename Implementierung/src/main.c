#include <stdlib.h>
#include <getopt.h>
#include <time.h>

#include "entropy.h"
#include "io_operations.h"

const char *optstring = ":-m:tah";

/*
    -l, --language => implementation language c|asm
	-m, --mode => run mode scalar|simd
    -i, --implementation => deg2|deg4|artanh|lookup
	-t, --time => calculate time that program takes
	-a, --accuracy => difference with double precision scalar entropy
	-h, --help 
*/

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
		{0, 0, 0, 0}};

	int optindex = 0;

	// Fetching option arguments
	while ((opt = getopt_long(argc, argv, optstring, long_options, &optindex)) != -1)
	{

		switch (opt)
		{
		case 'm':
			printf("mode : %s\n", optarg);
			break;
		case 't':
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
			printf("accuracy \n");
			break;
		case 'h':
			printf("Usage: entropy [options] file\n"
                   "\t-l, --language => implementation language c|asm\n"
                   "\t-m, --mode => run mode scalar|simd\n"
                   "\t-i, --implementation => deg2|deg4|artanh|lookup\n"
                   "\t-t, --time => calculate time that program takes\n"
                   "\t-a, --accuracy => difference with double precision scalar entropy\n"
                   "\t-h, --help\n"
                   );
			return EXIT_SUCCESS;

		case ':':
			fprintf(stderr, "Missing argumant for option -%c\n", optopt);
			__attribute__((fallthrough)); // Let fallthrough

		default:
			fprintf(stderr, "USAGE HERE\n");
			exit(EXIT_FAILURE);
			break;
		}
	}

	struct Handler handler;
	handler = handle_file(argv[optind]);
	if (handler.status != -1)
	{
		printf("Entropy is %f \n", scalar_entropy(handler.len, handler.data, log2approx_deg4));
		printf("Entropy is %f \n", precise_entropy(handler.len, handler.data));
		printf("Entropy simd is %f \n", simd_entropy(handler.simd_len, handler.data, log2_lookup_simd_asm));
		printf("Entropy simd is %f \n", simd_entropy(handler.simd_len, handler.data, log2approx_arctanh_simd_asm));
		free(handler.data);
	}

	// __m128 a = {-0.5, 0.0006, 0.99, 0.0625};
	// __m128 b = log2approx_arctanh_simd(a);
	// __m128 c = log2approx_arctanh_simd_asm(a);

	// printf("asm: %f %f %f %f  c: %f %f %f %f\n", c[0], c[1], c[2], c[3], b[0], b[1], b[2], b[3]);

	//printf("asm: %f  c: %f \n", log2_lookup_simd_asm(0.57), log2_lookup_simd(0.57));

	// // Fetching arguments
	// if (optind >= argc) {
	// 	fprintf(stderr, "Need to specify at least one input file!\n");
	// 	exit(EXIT_FAILURE);
	// }

	// }

	return EXIT_SUCCESS;
}
