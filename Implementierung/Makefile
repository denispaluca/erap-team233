CC = gcc

#					   limit to sse4.2 /             C11 / No x87 FPU / No MMX
CFLAGS = -O3 $(LDLIBS) -march=nehalem -mtune=generic -std=c11 -mno-80387 -mno-mmx -fsanitize=address -fno-omit-frame-pointer -fsanitize=undefined $(WARNING_FLAGS)
WARNING_FLAGS = -Wall -Wextra -Wpedantic -pedantic-errors -Wfatal-errors -Wcast-align -Wunused -Wshadow -Wcast-qual  -Wno-missing-braces
LDLIBS = -lm # Math Library

SRC_DIR = src
BUILD_DIR = build
TEST_DIR = test

LIB_FILES = $(shell find $(SRC_DIR) -name "*.c" -not -name "main.c" -o -name "*.S" -o -name "*.s")
HEADERS = $(wildcard $(SRC_DIR)/*.h)

PROGRAM = main
TEST = test

.PHONY: clean build_dir

$(PROGRAM): build_dir
	@$(CC) $(LIB_FILES) $(CFLAGS) $(SRC_DIR)/main.c -o $(BUILD_DIR)/$(PROGRAM)

test: build_dir
	@$(CC) $(LIB_FILES) $(CFLAGS) $(SRC_DIR)/test.c -o $(BUILD_DIR)/$(TEST)

build_dir:
	@mkdir -p build	

clean:
	rm -rf $(BUILD_DIR)/*