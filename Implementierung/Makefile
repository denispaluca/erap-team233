CC = gcc
CFLAGS = -O3 $(LDLIBS) -fsanitize=address -fno-omit-frame-pointer -fsanitize=undefined $(WARNING_FLAGS)
WARNING_FLAGS = -Wall -Wextra -Wpedantic -pedantic-errors -Wfatal-errors -Werror -Wcast-align -Wunused -Wshadow -Wcast-qual  -Wno-missing-braces
LDLIBS = -lm # Math Library

SRC_DIR = src
BUILD_DIR = build
TEST_DIR = test

LIB_FILES = $(shell find $(SRC_DIR) -name "*.c" -not -name "main.c" -o -name "*.S" -o -name "*.s")
HEADERS = $(wildcard $(SRC_DIR)/*.h)

PROGRAM = main
TEST = test

.PHONY: all clean

$(PROGRAM):
	$(CC) $(CFLAGS) $(LIB_FILES) $(SRC_DIR)/main.c -o $(BUILD_DIR)/$(PROGRAM)

test:
	$(CC) $(CFLAGS) $(LIB_FILES) test.c -o $(BUILD_DIR)/$(TEST)

clean:
	rm -rf $(BUILD_DIR)/*