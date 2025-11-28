# Makefile for DA7281 HAL unit tests (runs on PC without hardware)

CC = gcc
CFLAGS = -Wall -Wextra -g -O0
INCLUDES = -I../include

# Test executables
TESTS = test_without_hardware

all: $(TESTS)
	@echo "╔════════════════════════════════════════════╗"
	@echo "║  Building unit tests...                    ║"
	@echo "╚════════════════════════════════════════════╝"

test_without_hardware: test_without_hardware.c
	$(CC) $(CFLAGS) $(INCLUDES) -o $@ $<

run: all
	@echo ""
	@echo "╔════════════════════════════════════════════╗"
	@echo "║  Running unit tests...                     ║"
	@echo "╚════════════════════════════════════════════╝"
	@./test_without_hardware

clean:
	rm -f $(TESTS) *.o

.PHONY: all run clean

