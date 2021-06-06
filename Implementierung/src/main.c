#include <stdlib.h>

#include "entropy.h"
#include "io_operations.h"

int main(int argc, char *argv[])
{
	if (argc <= 1)
	{
		printf("To give input as a file include file path as a first argument.\n");
		printf("Example usage: ./a.out input.txt.\n");
		exit(1);
	}

	for (int32_t i = 1; i < argc ; ++i)
	{
		float entropy = file_entropy_c(argv[i]);
		if(entropy != -1)
		{
			printf("Entropy of a given probabilty distribution in file %s is: %f.\n", argv[i], entropy);
		}
	}
	return 0;
}
