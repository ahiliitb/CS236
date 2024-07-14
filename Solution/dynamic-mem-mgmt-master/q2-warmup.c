#include <stdlib.h>
#include <stdio.h>
#include <sys/mman.h>

int main(){
    char* reg = mmap(NULL, 4096, PROT_READ | PROT_WRITE, MAP_PRIVATE | MAP_ANONYMOUS, 0, 0);
    int x;
    scanf("%d\n", &x);
}