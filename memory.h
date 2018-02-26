#ifndef MEMORY_H_
#define MEMORY_H_ 

/* Statistics about the simulation result. */
typedef struct Statistics
{
    int writes_to_disk, /* Number of disk writes */
        page_faults; /* Number of pages faults */
} Statistics;

/* Internal state of memory. */
typedef struct Memory Memory;

Memory* memInit(char *algo, int nframes);

void freeMem(Memory *mem);

void memClockInturrupt(Memory *mem);

/* Simulates access to memory, both read and write (indicated by rw). */
void accessMem(Memory *mem, unsigned addr, char rw);

void printFrames(Memory *mem);

Statistics memStats(Memory *mem);

int memTime(Memory *mem);

#endif
