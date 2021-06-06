#include "io_operations.h"

// This function determines the size of inputs in the given file name.
int size_file(const char* file_name)
{
	FILE* input_file;
	input_file = fopen(file_name, "r");

	if (input_file == NULL)
	{
		printf("Error occured while trying to open the input file: %s.\n", file_name);
		printf("Skipping the file %s. \n", file_name);
		return 0;
	}

	size_t len = 0;
	float tmp = 0;
	float sum = 0;

	// First reading the file until an error occurs or until reach the end of line.
	while (fscanf(input_file, "%f", &tmp) == 1)
	{
		++len;
		if (tmp < 0 || tmp > 1)
		{
			printf("There are negative values in your input file: %s.\n", file_name);
			printf("Skipping the file %s.\n", file_name);
			fclose(input_file);
			return 0;
		}
		sum += tmp;
	}

	// Check if sum represents probability distrubtion with given inaccuracy of floating points.
	if (sum > UPPER_LIMIT || sum < LOWER_LIMIT)
	{
		printf("Given inputs does not represent probability distribution.\n");
		printf("Sum of the inputs is: %f but expected it was between %f and %f.\n", sum, LOWER_LIMIT, UPPER_LIMIT);
		printf("Skipping the file %s.\n", file_name);
		fclose(input_file);
		return 0;
	}
	fclose(input_file);
	return len;
}

float* read_file(size_t len, const char* file_name)
{

	// allocate enough spaces to store every input.
	float* inputs = malloc(len * sizeof(float));

	if (inputs == NULL)
	{
		printf("Could not allocated enough memory to store every input.\n");
		printf("Number of elements to store: %zu.\n", len);
		printf("Skipping the file %s.\n", file_name);
		return NULL;
	}

	FILE* input_file;
	input_file = fopen(file_name, "r");

	if (input_file == NULL)
	{
		printf("Error occured while trying to open the input file: %s.\n", file_name);
		printf("Skipping the file %s. \n", file_name);
		return NULL;
	}

	// take inputs from the file and store it in inputs.
	for (size_t i = 0 ; i < len; ++i)
	{
		fscanf(input_file, "%f", &inputs[i]);
	}

	// free resources
	fclose(input_file);

	return inputs;
}