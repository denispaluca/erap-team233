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

		size_t len = size_file(argv[i]);
		float* data = NULL;
		if (len != 0)
		{
			data = read_file(len, argv[i]);
		}
		if (data != NULL)
		{

			/*for (size_t j = 0 ; j < len ; ++j)
			{
				printf("Number is: %f \n", data[j]);
			}*/

			float entropy = scalar_entropy(len, data);
			printf("Entropy of a given probabilty distribution in file %s is: %f.\n", argv[i], entropy);
			entropy = entropy_asm(len, data);
			printf("Entropy of a given probabilty distribution in file %s is: %f.\n", argv[i], entropy);
			entropy = entropy_asm_simd(len, data);
			printf("Entropy of a given probabilty distribution in file %s is: %f.\n", argv[i], entropy);
			free(data);
			return entropy;
		}
		
		

	}
	return 0;
}
