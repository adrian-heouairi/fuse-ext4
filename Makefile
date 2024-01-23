CC=gcc
CFLAGS=-Wall -g -pedantic -Wextra -Werror $(shell pkg-config fuse3 --cflags)
LDFLAGS=$(shell pkg-config fuse3 --libs)

all: fuse-ext4

fuse-ext4: main.c
	$(CC) $(CFLAGS) -o $@ main.c $(LDFLAGS)

run: fuse-ext4
	mkdir -pv mntpt
	fusermount -u mntpt || true
	./fuse-ext4 mntpt
