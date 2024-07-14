#include "alloc.h"
#include <sys/mman.h>
#include <fcntl.h>
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>


void *initaddr;
void *curraddr;
size_t size = 4096;
struct page *alloc_hist; 


int init_alloc()
{
    initaddr = mmap(NULL, size, PROT_READ | PROT_WRITE, MAP_PRIVATE | MAP_ANONYMOUS, -1, 0);
    if (initaddr == MAP_FAILED) {
        return -1;
    }
    alloc
    return 0;
}

int cleanup()
{
    if (munmap(initaddr, size) == -1) {
        return -1; // Return non-zero error code
    }
    return 0;
}

char *alloc(int buffer_size)
{
    if(buffer_size%8 != 0)
    {
        return NULL;
    }

}

void dealloc(char *)
{

}