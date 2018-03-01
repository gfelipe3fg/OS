#ifndef MEMORY_H
#define MEMORY_H

#include "simulator.h"

typedef struct PageFrame{
    char rw;         // 0 for 'r', 1 for 'w'
    unsigned addr;   // Virtual address
    unsigned int pos;  // 0 for local, 1 for disk
    unsigned int frame_number; 
    unsigned int accessed_w;  // If it has ever been accessed to write
}PageFrame;

typedef struct PageTable{
    PageFrame *page_frames;
    int nframes;
    int numAvalFrames;
    int numUnavalFrames;
}PageTable;

PageTable* mem_init(int nFrames);

int findPage(int addr,PageTable * mem);
void loadPage(PageTable * mem, TraceEntry entry);
void accessMem(PageTable *mem, TraceEntry entry);
void print_memory(PageTable *mem);
void print_results(PageTable *mem);
void freeMem(PageTable *mem);
#endif

