#include <stdlib.h>
#include <getopt.h>

#include "entropy.h"
#include "io_operations.h"

const char* optstring = "-abc";

/*

*/

int main(int argc, char *argv[]) {

	
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

	int opt;
	//char** input_files[argc];

	// Fetching option arguments
	while( (opt = getopt(argc, argv, optstring)) != -1 ) {

		switch (opt)
		{
		case 'a' :
		case 'b' :
		case 'c' :
		break;

		case 1 :
			// Add optarg to input files
			printf("%p\n", (void*) optarg);
			break;

		
		default:
			fprintf(stderr, "ananı bacını\n");
			exit(EXIT_FAILURE);
			break;
		}

	}

	// Fetching arguments
	if (optind >= argc) {
		fprintf(stderr, "Need to specify at least one input file!");
		exit(EXIT_FAILURE);
	}

	
	// if (argc <= 1)
	// {
	// 	printf("To give input as a file include file path as a first argument.\n");
	// 	printf("Example usage: ./a.out input.txt.\n");
	// 	exit(1);
	// }

	// for (int32_t i = 1; i < argc ; ++i)
	// {
	// 	float entropy = file_entropy_c(argv[i]);
	// 	if(entropy != -1)
	// 	{
	// 		printf("Entropy of a given probabilty distribution in file %s is: %f.\n", argv[i], entropy);
	// 	}
	// }
	return EXIT_FAILURE;
}
