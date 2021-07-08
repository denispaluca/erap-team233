#include "io_operations.h"

// This function determines the size of inputs in the given file name.
static size_t size_file(const char *file_name)
{
	FILE *input_file;
	input_file = fopen(file_name, "r");

	if (input_file == NULL)
	{
        printf("Error occurred while trying to open the input file: %s.\n", file_name);
        return 0;
    }

	size_t len = 0;
	float tmp = 0;

	// First reading the file until an error occurs or until reach the end of line.
	while (fscanf(input_file, "%f", &tmp) == 1)
	{
		++len;
	}
	fclose(input_file);
	return len;
}

static float *read_file(size_t len, const char *file_name)
{
	// allocate enough spaces to store every input.
	size_t align = len + (4 - len % 4) % 4;
	float *inputs = aligned_alloc(16, align * sizeof(float));

	if (inputs == NULL)
	{
		printf("Could not allocated enough memory to store every input.\n");
		printf("Number of elements to store: %zu.\n", len);
		return NULL;
	}

	FILE *input_file;
	input_file = fopen(file_name, "r");

	if (input_file == NULL) {
        printf("Error occurred while trying to open the input file: %s.\n", file_name);
        free(inputs);
        return NULL;
    }

	// take inputs from the file and store it in inputs.
	for (size_t i = 0; i < len; ++i)
	{
		if (!fscanf(input_file, "%f", &inputs[i]))
		{
			free(inputs);
			return NULL;
		}
	}
	for (size_t i = len; i < align; ++i)
	{
		inputs[i] = 0.0f;
	}

	// free resources
	fclose(input_file);

	return inputs;
}

struct Handler handle_file(const char *file_name)
{
	struct Handler handler;
	handler.len = 0;
	handler.data = NULL;
	handler.status = -1;
	size_t len = size_file(file_name);
	if (len == 0)
	{
		printf("No numbers are read from the file %s.\n", file_name);
		return handler;
	}
	handler.data = read_file(len, file_name);
	if (handler.data == NULL)
	{
		return handler;
	}
	handler.len = len;
	handler.status = 0;
	return handler;
}
