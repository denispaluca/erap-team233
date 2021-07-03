CC = gcc

#		math library	           enable sse4.2 / C11       No x87 / No MMX  / CLOCK_MONOTIC  / target assembly 64 bit generic until sse4.2 without mmx and x87
CFLAGS = -O3 -lm -no-pie -mtune=generic -msse4.2 -std=c11 -mno-80387 -mno-mmx -D_POSIX_C_SOURCE=199309L -Wa,-march=generic64+sse4.2+nommx+no87
# Math Library, without no-pie fails to use math library functions in assembly

VERBOSE_FLAGS = -g -fsanitize=address -fno-omit-frame-pointer -fsanitize=undefined $(WARNING_FLAGS)
WARNING_FLAGS = -Wall -Wextra -Wpedantic -pedantic-errors -Wfatal-errors -Wcast-align -Wunused -Wshadow -Wcast-qual -Wno-missing-braces

SRC_DIR = src
BUILD_DIR = build
TEST_DIR = tests

LIB_FILES = $(shell find $(SRC_DIR) -name "*.c" -not -name "main.c" -o -name "*.S" -o -name "*.s")
TEST_FILES = $(shell find $(TEST_DIR) -name "*.c" -not -name "main.c" -o -name "*.S" -o -name "*.s")

PROGRAM = entropy
TEST = test

.PHONY: clean build_dir release perf-cache perf-branch

#default target is release mode
$(PROGRAM): build_dir 
	$(CC) $(LIB_FILES) $(CFLAGS) $(SRC_DIR)/main.c -o $(BUILD_DIR)/$(PROGRAM)

debug: build_dir
	@$(CC) $(LIB_FILES) $(CFLAGS) $(VERBOSE_FLAGS) $(SRC_DIR)/main.c -o $(BUILD_DIR)/$(PROGRAM)

test: build_dir
	@$(CC) $(TEST_FILES) $(LIB_FILES) $(CFLAGS) -o $(BUILD_DIR)/$(TEST)

# Need to specify program argumets via "make perf-cache|perf-branch args="<ARGS HERE>" "
perf-cache: $(PROGRAM)
	@perf stat -e cache-misses ./$(BUILD_DIR)/$(PROGRAM) $(args)

perf-branch: $(PROGRAM)
	@perf stat -e branch-misses ./$(BUILD_DIR)/$(PROGRAM) $(args)

build_dir:
	@mkdir -p build	

clean:
	rm -rf $(BUILD_DIR)/*
