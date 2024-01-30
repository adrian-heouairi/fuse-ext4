# Compiler and Flags
CC=gcc
CFLAGS=-Wall -g -pedantic -Wextra -Werror $(shell pkg-config fuse3 --cflags)
LDFLAGS=$(shell pkg-config fuse3 --libs)

# Target binary
TARGET=fuse-ext4

# Source files
SOURCES=main.c

# Default target
all: $(TARGET)

# Target to check and install dependencies
deps:
	@echo "Checking for dependencies..."
	@if pkg-config --exists fuse3; then \
		echo "fuse3 is installed."; \
	else \
		echo "fuse3 not found. Attempting to install libfuse3-dev..."; \
		sudo apt-get install libfuse3-dev; \
	fi
	@echo "Dependency check complete."

# Compiling the program
$(TARGET): deps $(SOURCES)
	$(CC) $(CFLAGS) -o $@ $(SOURCES) $(LDFLAGS)

# Target to run the program
run: $(TARGET)
	@mkdir -pv mntpt
	fusermount -u mntpt || true
	@./$(TARGET) mntpt

# Clean target
clean:
	@rm -f $(TARGET)
	@echo "Cleaned."

.PHONY: all deps run clean
