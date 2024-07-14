#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <sys/shm.h>
#include <sys/stat.h>
#include <sys/mman.h>
#include <unistd.h>
#include <string.h>

int main()
{
	const char* fifo = "/tmp/fifo";
	mkfifo(fifo, 0666);
	int fd = open(fifo, O_RDONLY);

	char buffer[100];
	read(fd, buffer, 100);
	printf("Received: %s\n", buffer);
}
