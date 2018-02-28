#ifndef MEMORY_H
#define MEMORY_H

typedef struct PageFrame{
    char rw;         // 0 for 'r', 1 for 'w'
    unsigned addr;   // Virtual address
    unsigned int pos;  // 0 for local, 1 for disk
    unsigned int frame_number;
}PageFrame;

typedef struct PageTable{
    PageFrame *page_frames;
    int nframes;
    int numAvalFrames;
    int numUnavalFrames;
}PageTable;

PageTable* mem_init(int nFrames);

void print_memory(PageTable *mem);
void freeMem(PageTable *mem);
#endif

