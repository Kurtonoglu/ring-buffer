# Ring buffer — build, test, static analysis and coverage

CC       := gcc
CSTD     := -std=c99
WARNINGS := -Wall -Wextra -Werror -Wconversion -Wshadow
INCLUDES := -Iinclude -Iexternal/Unity/src
CFLAGS   := $(CSTD) $(WARNINGS) $(INCLUDES)

BUILD_DIR := build
TARGET    := $(BUILD_DIR)/test_ring_buffer

SRC       := src/ring_buffer.c
TEST_SRC  := tests/test_ring_buffer.c
UNITY_SRC := external/Unity/src/unity.c

COVERAGE_FLAGS := --coverage -O0 -g

.PHONY: all test coverage analyze clean

all: test

## Build and run the unit tests
test: $(BUILD_DIR)
	$(CC) $(CFLAGS) $(SRC) $(TEST_SRC) $(UNITY_SRC) -o $(TARGET)
	./$(TARGET)

## Run the tests instrumented for coverage and report line/branch coverage
coverage: $(BUILD_DIR)
	$(CC) $(CFLAGS) $(COVERAGE_FLAGS) $(SRC) $(TEST_SRC) $(UNITY_SRC) \
		-o $(BUILD_DIR)/test_coverage
	./$(BUILD_DIR)/test_coverage
	gcovr --root . --filter src/ --txt-metric branch --print-summary \
		--html-details $(BUILD_DIR)/coverage.html

## Static analysis
analyze:
	cppcheck --enable=warning,style,performance,portability \
		--std=c99 --inline-suppr --error-exitcode=1 \
		--suppress=missingIncludeSystem \
		-Iinclude src/ tests/

$(BUILD_DIR):
	mkdir -p $(BUILD_DIR)

clean:
	rm -rf $(BUILD_DIR) *.gcda *.gcno *.gcov