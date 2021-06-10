#include <stdlib.h>
#include <getopt.h>

#include "entropy.h"
#include "io_operations.h"

const char *optstring = ":-m:tah";

/*
	JUST A DRAFT

	-m, --mode => run mode asm|simd|c
	-t, --time => calculate time that program takes
	-a, --accuracy => accuracy
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
		{"mode", required_argument, 0, 'm'},
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
			break;
		case 'a':
			printf("accuracy \n");
			break;
		case 'h':
			printf("help \n");
			break;

			// ===========================================================
			// TODO: implement functionality
			// ===========================================================

		case ':':
			fprintf(stderr, "Missing argumant for option -%c\n", optopt);
			__attribute__((fallthrough)); // Let fallthrough

		default:
			fprintf(stderr, "USAGE HERE\n");
			exit(EXIT_FAILURE);
			break;
		}
	}

	__m128 a = {-0.5, 0.0006, 0.99, 0.0625};
	__m128 b = log2approx_arctanh_simd(a);
	__m128 c = log2approx_arctanh_simd_asm(a);

	printf("asm: %f %f %f %f  c: %f %f %f %f\n", c[0], c[1], c[2], c[3], b[0], b[1], b[2], b[3]);

	// // Fetching arguments
	// if (optind >= argc) {
	// 	fprintf(stderr, "Need to specify at least one input file!\n");
	// 	exit(EXIT_FAILURE);
	// }

	for (; optind < argc; ++optind)
	{
		size_t len = size_file(argv[optind]);
		float *data = NULL;
		if (len != 0)
		{
			data = read_file(len, argv[optind]);
		}
		if (data != NULL)
		{

			float entropy = scalar_entropy(len, data);
			printf("Entropy is: %f \n",entropy);
			len = len + (4-len%4)%4;
			entropy = simd_entropy(len,data,log2approx_arctanh_simd);
			printf("Simd Entropy is: %f \n",entropy);


			free(data);
			
		}
		float entropy = file_entropy_c(argv[optind]);

		fprintf(stderr, "Entropy of a given probabilty distribution in file %s is: %f\n", argv[optind], entropy);
	}

	// }

	return EXIT_SUCCESS;
}
