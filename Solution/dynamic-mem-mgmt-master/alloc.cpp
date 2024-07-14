#include "alloc.h"
#include <map>

using namespace std;

char* mem;

#define NUM_ELEM PAGESIZE/MINALLOC

bool bitmap[NUM_ELEM];
map<unsigned long, int> ptr_to_sz;

int init_alloc(){
    mem = (char*)mmap(NULL, PAGESIZE, PROT_READ | PROT_WRITE, MAP_PRIVATE | MAP_ANONYMOUS, 0, 0);
    memset(mem, 0, PAGESIZE);
    for(int i = 0; i < NUM_ELEM; i++){
        bitmap[i] = true;
    }
    if(!mem){
        return 1;
    }
    return 0;
}

int cleanup(){
    ptr_to_sz.clear();
    for(int i = 0; i < NUM_ELEM; i++){
        bitmap[i] = true;
    }
    return munmap(mem, PAGESIZE);
}

char* alloc(int sz){
    if(sz % MINALLOC != 0){
        return NULL;
    }
    int req = sz/MINALLOC;
    int minsz = INT32_MAX;
    int tmpminsz = -1;
    int tmpoff = -1;
    int off = -1;
    bool flag = false;
    for(int i = 0; i < NUM_ELEM; i++){
        if(!flag){
            if(bitmap[i]){
                flag = true;
                tmpoff = i;
                tmpminsz = 1;
            }
        }
        else{
            if(bitmap[i]){
                tmpminsz++;
            }
            else{
                if((minsz >= tmpminsz) && (tmpminsz >= req)){
                    minsz = tmpminsz;
                    off = tmpoff;
                    flag = false;
                }
            }
        }
    }
    if((minsz >= tmpminsz) && (tmpminsz >= req)){
        minsz = tmpminsz;
        off = tmpoff;
    }
    if(off == -1){
        return NULL;
    }
    ptr_to_sz[(unsigned long)(mem + off*MINALLOC)] = req;
    for(int i = off; i < off + req; i++){
        bitmap[i] = false;
    }
    return (mem + off * MINALLOC);
}

void dealloc(char* ptr){
    if(ptr_to_sz.count((unsigned long)ptr) == 0){
        return;
    }
    int num_blocks = ptr_to_sz[(unsigned long)ptr];
    int off = ((unsigned long)(ptr - mem) & 0xfff)/MINALLOC;
    for(int i = off; i < off + num_blocks; i++){
        bitmap[i] = true;
    }
    ptr_to_sz.erase((unsigned long)ptr);
    memset(ptr, 0, num_blocks * MINALLOC);
}