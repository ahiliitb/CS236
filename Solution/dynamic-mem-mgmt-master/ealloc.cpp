#include "alloc.h"
#include <map>

using namespace std;

#define MAXPAGES 4

char* mem[MAXPAGES];

#define NUM_ELEM PAGESIZE/MINALLOC

bool bitmap[MAXPAGES][NUM_ELEM];
map<unsigned long, pair<int, int>> ptr_to_sz_pg;

int init_alloc(){
    for(int i = 0; i < 4; i++){
        for(int j = 0; j < NUM_ELEM; j++){
            bitmap[i][j] = true;
        }
    }
    return 0;
}

int cleanup(){
    ptr_to_sz_pg.clear();
    for(int i = 0; i < 4; i++){
        for(int j = 0; j < NUM_ELEM; j++){
            bitmap[i][j] = true;
        }
    }
    return 0;
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
    int pg_ind = -1;
    bool flag = false;
    for(int j = 0; j < 4; j++){
        if(mem[j]){
            for(int i = 0; i < NUM_ELEM; i++){
                if(!flag){
                    if(bitmap[j][i]){
                        flag = true;
                        tmpoff = i;
                        tmpminsz = 1;
                    }
                }
                else{
                    if(bitmap[j][i]){
                        tmpminsz++;
                    }
                    else{
                        if((minsz >= tmpminsz) && (tmpminsz >= req)){
                            minsz = tmpminsz;
                            off = tmpoff;
                            flag = false;
                            pg_ind = j;
                        }
                    }
                }
            }
            if((minsz >= tmpminsz) && (tmpminsz >= req)){
                minsz = tmpminsz;
                off = tmpoff;
                pg_ind = j;
            }
        }
        else{
            if(off == -1){
                mem[j] = (char*)mmap(NULL, PAGESIZE, PROT_READ | PROT_WRITE, MAP_PRIVATE | MAP_ANONYMOUS, 0, 0);
                off = 0;
                pg_ind = j;
            }
        }
    }
    if(off == -1){
        return NULL;
    }
    ptr_to_sz_pg[(unsigned long)(mem[pg_ind] + off * MINALLOC)] = {req, pg_ind};
    for(int i = off; i < off + req; i++){
        bitmap[pg_ind][i] = false;
    }
    return (mem[pg_ind] + off * MINALLOC);
}

void dealloc(char* ptr){
    if(ptr_to_sz_pg.count((unsigned long)ptr) == 0){
        return;
    }
    auto nb_pgs = ptr_to_sz_pg[(unsigned long)ptr];
    int off = ((unsigned long)(ptr - mem[nb_pgs.second]) & 0xfff)/MINALLOC;
    for(int i = off; i < off + nb_pgs.first; i++){
        bitmap[nb_pgs.second][i] = true;
    }
    ptr_to_sz_pg.erase((unsigned long)ptr);
    memset(ptr, 0, nb_pgs.first * MINALLOC);
}