#ifndef SIMULATOR_H
#define SIMULATOR_H

//#include "memory.h"

#define MAX_ARGV 301 // 300 characters max
#define MAX_ACCESSES 1000001

typedef struct Statistics{
    int writes_to_disk;
    int reads_from_disk;
    int num_mem_accesses;
}Statistics;

typedef struct TraceEntry{
    char rw;
    unsigned addr;
}TraceEntry;

typedef struct Options{
    char filename[MAX_ARGV];
    char method[MAX_ARGV];
    int frames_input;
    int debug_mode;
}Options;

void runSim(Options ops);

void loadEntries(TraceEntry * entries,Options ops,Statistics* stats);

#endif
