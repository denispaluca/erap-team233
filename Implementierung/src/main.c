#include <stdlib.h>
#include <getopt.h>

#include "entropy.h"
#include "io_operations.h"

const char* optstring = ":+m:ta";

/*
	JUST A DRAFT

	-m => run mode asm|simd|c
	-t => calculate time that program takes
	-a => accuracy
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

	// Fetching option arguments
	while( (opt = getopt(argc, argv, optstring)) != -1 ) {

		switch (opt)
		{
		case 'm' :
		case 't' :
		case 'a' :
		break;

		// ===========================================================
		// TODO: implement functionality
		// ===========================================================
		
		case ':' :
			fprintf(stderr, "Missing argumant for option %c\n", optopt);
			break;

		default:
			fprintf(stderr, "USAGE HERE\n");
			exit(EXIT_FAILURE);
			break;
		}

	}

	// Fetching arguments
	if (optind >= argc) {
		fprintf(stderr, "Need to specify at least one input file!\n");
		exit(EXIT_FAILURE);
	}


	for (; optind < argc ; ++optind) {
		int size = size_file(argv[optind]);
		float entropy = scalar_entropy(size, read_file(size, argv[optind]));
		if(entropy != -1) {
			fprintf(stderr, "Entropy of a given probabilty distribution in file %s is: %f\n", argv[optind], entropy);
		}
	
	}

	return EXIT_SUCCESS;
}
