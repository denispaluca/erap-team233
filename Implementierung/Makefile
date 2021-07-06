CC = gcc

#		math library	           enable sse4.2 / C11       No x87 / No MMX  / CLOCK_MONOTIC  / target assembly 64 bit generic until sse4.2 without mmx and x87
CFLAGS = -O3 -lm -no-pie -mtune=generic -msse4.2 -std=c11 -mno-80387 -mno-mmx -D_POSIX_C_SOURCE=199309L -Wa,-march=generic64+sse4.2+nommx+no87
# Math Library, without no-pie fails to use math library functions in assembly

VERBOSE_FLAGS = -g -fsanitize=address -fno-omit-frame-pointer -fsanitize=undefined  $(WARNING_FLAGS)
WARNING_FLAGS = -Wall -Wextra -Wpedantic -pedantic-errors -Wfatal-errors -Wcast-align -Wunused -Wshadow -Wcast-qual -Wno-missing-braces

SRC_DIR = src

FILES = $(shell find . -name "*.c" -o -name "*.S" -o -name "*.s")

PROGRAM = entropy

.PHONY: clean perf-cache perf-branch

#default target is release mode
$(PROGRAM): $(FILES)
	@$(CC) $(FILES) $(CFLAGS) -o $(PROGRAM)

debug: $(FILES)
	@$(CC) $(FILES) $(CFLAGS) $(VERBOSE_FLAGS) -o $(PROGRAM)

# Need to specify program argumets via "make perf-cache|perf-branch args="<ARGS HERE>" "
perf-cache: $(PROGRAM)
	perf stat -e cache-misses ./$(PROGRAM) $(args)

perf-branch: $(PROGRAM)
	perf stat -e branch-misses ./$(PROGRAM) $(args)

clean:
	rm $(PROGRAM)
