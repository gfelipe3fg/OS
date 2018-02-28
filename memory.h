#ifndef MEMORY_H_
#define MEMORY_H_ 

/* Statistics about the simulation result. */
typedef struct Statistics
{
    int writes_to_disk, /* Number of disk writes */
     page_faults,/* Number of pages faults */
    reads_to_disk;
} Statistics;

typedef struct PageFrame {
    int ref,
        mod,  // 1 if page has been modified (has write access)
        last_element,
        first_element;
    int virtual_index;
}PageFrame;

typedef enum{
    FIFO,LRU,VMS
}Algo;

/* Internal state of memory. */
typedef struct Memory{
    Algo algo;
    PageFrame *frames;
    int time_counter,
        page_size,
        numPF,
        numUPF; // Utilized page frame
    Statistics stats;
}Memory;

Memory* memInit(char *algo, int nframes);

void freeMem(Memory *mem);

//void memClockInturrupt(Memory *mem);

/* Simulates access to memory, both read and write (indicated by rw). */
void accessMem(Memory *mem, unsigned addr, char rw);

void printFrames(Memory *mem);

int memTime(Memory *mem);

#endif
