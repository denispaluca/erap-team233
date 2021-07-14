CC = gcc

# math library / enable sse4.2 / C11 / CLOCK_MONOTIC  
CFLAGS = -O3 -lm -no-pie -msse4.2 -std=c11 -D_POSIX_C_SOURCE=199309L
# Math Library, without no-pie fails to use math library functions in assembly

VERBOSE_FLAGS = -g -fsanitize=address -fno-omit-frame-pointer -fsanitize=undefined  $(WARNING_FLAGS)
WARNING_FLAGS = -Wall -Wextra -Wpedantic -pedantic-errors -Wfatal-errors -Wcast-align -Wunused -Wshadow -Wcast-qual -Wno-missing-braces

C_FILES = $(shell find . -name "*.c" -o -name "*.S" -o -name "*.s")
H_FILES = $(shell find . -name "*.h") # also track changes in header files

PROGRAM = entropy

.PHONY: clean debug perf-cache perf-branch

#default target is release mode
$(PROGRAM): $(C_FILES) $(H_FILES)
	@$(CC) $(C_FILES) $(CFLAGS) -o $(PROGRAM)

debug: $(C_FILES) $(H_FILES)
	@$(CC) $(C_FILES) $(CFLAGS) $(VERBOSE_FLAGS) -o $(PROGRAM)

# Need to specify program arguments via "make perf-cache|perf-branch args="<ARGS HERE>" "
perf-cache: debug
	perf stat -e cache-misses ./$(PROGRAM) $(args)

perf-branch: debug
	perf stat -e branch-misses ./$(PROGRAM) $(args)

clean:
	rm $(PROGRAM)
