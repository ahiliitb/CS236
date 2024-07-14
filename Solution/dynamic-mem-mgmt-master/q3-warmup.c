#include <stdlib.h>
#include <stdio.h>
#include <sys/mman.h>

int main(){
    char* reg = mmap(NULL, 4096, PROT_READ | PROT_WRITE, MAP_PRIVATE | MAP_ANONYMOUS, 0, 0);
    for(int i = 1; i < 4096; i++){
        reg[i] = 0;
    }
    int x;
    scanf("%d\n", &x);
}