#ifndef SIMULATOR_H_
#define SIMULATOR_H_
#include "memory.h"

#define MAX_MEM_ACCESS 1000005
#define MAX_ARGV 301

typedef struct MemAccess{
    char rw;
    unsigned addr;
}MemAccess;

typedef struct Simulator {
    MemAccess memAccesses[MAX_MEM_ACCESS];
    int numMemAccesses;
    Memory *mem;
}Simulator;

typedef struct Options {
    char filename[MAX_ARGV];
    char algo[MAX_ARGV];
    int nFrames;
    int debug_mode;
} Options;

/* Parses the arguments passed in the command line
and returns a struct representing the options. */
Options argsOptions(int argc, const char **argv);

/* Initializes the simulator with the chosen options, loading
the accesses of an input file. */
Simulator *simInit(Options opts);

/* Executes simulation. */
void runSim(Simulator *sim);

/* Prints results of simulation. */
void printSimResult(Simulator *sim);

/* De-allocates resources used by the simulator. */
void freeSim(Simulator *sim);

#endif
