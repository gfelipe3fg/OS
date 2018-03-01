#ifndef MEMORY_H
#define MEMORY_H

#include "simulator.h"

typedef enum{
    FIFO,LRU,VMS
}Algo;

typedef struct PageFrame{
    char rw;         // 0 for 'r', 1 for 'w'
    unsigned addr;   // Virtual address
    unsigned int pos;  // 0 for local, 1 for disk
    unsigned int frame_number; 
    unsigned int accessed_w;  // If it has ever been accessed to write
}PageFrame;

typedef struct Queue{
    PageFrame front,rear;
    int front_idx;
    int rear_idx;
    int size; //Size used so far
    unsigned capacity;
    PageFrame * array;
}queue;

typedef struct PageTable{
    Algo algo;
    PageFrame *page_frames;
    int nframes;
    int numAvalFrames;
    queue *m_queue;
}PageTable;


queue * createQueue(unsigned capacity);

PageTable* mem_init(int nFramesi,char * algo);

PageFrame fifo(PageTable *mem);

void removePage(PageTable *mem, TraceEntry entry);
int findPage(int addr,PageTable * mem);
void loadPage(PageTable * mem, TraceEntry entry);
void accessMem(PageTable *mem, TraceEntry entry);
void print_memory(PageTable *mem);
void print_results(PageTable *mem);
void freeMem(PageTable *mem);

int isFull(queue * q);
int isEmpty(queue * q);
void enqueue(queue * q, PageFrame frame);
PageFrame dequeue(queue *q);
int front(queue *q);
int rear(queue *q);
void printQueue(queue *q);
#endif

