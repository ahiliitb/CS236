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
	int fd = open(fifo, O_WRONLY);

	for(;;)
	{
		char* hello;
		scanf("%s", hello);
		write(fd, hello, strlen(hello) + 1);
	}
}
