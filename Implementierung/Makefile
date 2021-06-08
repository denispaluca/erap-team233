CC = gcc

#					   limit to sse4.2 /             C11 / No x87 FPU / No MMX
CFLAGS = -O3 $(MATHLIB) -march=nehalem -mtune=generic -std=c11 -mno-80387 -mno-mmx
MATHLIB = -lm -no-pie # Math Library, without no-pie fails to use math library functions in assembly

VERBOSE_FLAGS = -g -fsanitize=address -fno-omit-frame-pointer -fsanitize=undefined $(WARNING_FLAGS)
WARNING_FLAGS = -Wall -Wextra -Wpedantic -pedantic-errors -Wfatal-errors -Wcast-align -Wunused -Wshadow -Wcast-qual  -Wno-missing-braces

SRC_DIR = src
BUILD_DIR = build
TEST_DIR = tests

LIB_FILES = $(shell find $(SRC_DIR) -name "*.c" -not -name "main.c" -o -name "*.S" -o -name "*.s")
HEADERS = $(wildcard $(SRC_DIR)/*.h)

PROGRAM = entropy
TEST = test

.PHONY: clean build_dir release perf-cache perf-branch

$(PROGRAM): build_dir
	@$(CC) $(LIB_FILES) $(CFLAGS) $(VERBOSE_FLAGS) $(SRC_DIR)/main.c -o $(BUILD_DIR)/$(PROGRAM)

test: build_dir
	@$(CC) $(LIB_FILES) $(CFLAGS) $(VERBOSE_FLAGS) $(TEST_DIR)/test.c -o $(BUILD_DIR)/$(TEST)

release: $(PROGRAM)
	$(CC) $(LIB_FILES) $(CFLAGS) $(SRC_DIR)/main.c -o $(BUILD_DIR)/$(TEST)

# Need to specify program argumets via "make perf-cache|perf-branch args="<ARGS HERE>" "
perf-cache: $(PROGRAM)
	@perf stat -e cache-misses ./$(BUILD_DIR)/$(PROGRAM) $(args)

perf-branch: $(PROGRAM)
	@perf stat -e branch-misses ./$(BUILD_DIR)/$(PROGRAM) $(args)

build_dir:
	@mkdir -p build	

clean:
	rm -rf $(BUILD_DIR)/*
