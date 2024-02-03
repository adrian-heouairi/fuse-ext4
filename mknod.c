#include <sys/stat.h>
#include <stdio.h>
#include <string.h>

int main(int args, char **argv) {
	char buffer[16384] = "mntpt";

	//open("../mntpt/x", O_CREAT | O_RDONLY)
	int ret = mknod(strcat(buffer, argv[1]), S_IFREG | 0777, 0);

	printf("mknod returned %d (0 == success)\n", ret);
}
