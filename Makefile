# Compiler and flags
CC=gcc
CFLAGS=-Wall -g -pedantic $(shell pkg-config fuse3 --cflags) # -Wextra -Werror
LDFLAGS=$(shell pkg-config fuse3 --libs)

# Target binary
TARGET=fe4

# Source files
#SOURCES=data_structures.c main.c utils.c sprintstatf.c
SOURCES=*.c

# Default target
all: $(TARGET)

# Target to check and install dependencies
deps:
	echo "Checking for dependencies..."
	if pkg-config --exists fuse3; then \
		echo "fuse3 is installed."; \
	else \
		echo "fuse3 not found. Attempting to install Ubuntu package libfuse3-dev..."; \
		sudo apt-get install libfuse3-dev; \
	fi
	echo "Dependency check complete."

# Compiling the program
$(TARGET): deps $(SOURCES)
	$(CC) $(CFLAGS) -o $@ $(SOURCES) $(LDFLAGS)

# Target to run the program
run: $(TARGET)
	mkdir -pv mntpt
	killall $(TARGET) || true
	./$(TARGET) mntpt

# Clean target
clean:
	killall $(TARGET) || true
	sleep 1
	rm -vf $(TARGET)
	rmdir -v mntpt
	echo "Cleaned."

test: Test/mknod.c
	$(CC) $(CFLAGS) -o Test/mknod Test/mknod.c $(LDFLAGS)

.PHONY: all deps run clean
