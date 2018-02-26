#include "simulator.h"
#include "memory.h"
#include "util.h"

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <assert.h>

typedef struct MemAccess {
    char rw;
    unsigned addr;
} MemAccess;

struct Simulator {
    MemAccess memAccesses[MAX_MEM_ACCESS];
    int numMemAccesses;
    Memory *mem;
};

/********** Functions **********/

Options argsOptions(int argc, const char **argv) {
    if (argc == 4 || argc == 5) {
        Options opts;
        strcpy(opts.filename, argv[1]);
        opts.nFrames = atoi(argv[2]);
        strcpy(opts.algo, argv[3]);
        if (argc == 5) opts.debug_mode = atoi(argv[4]);
        else opts.debug_mode = 0;
        return opts;
    } else {
        printf("ERROR: Incorrect number of Arguments (%d). Expected 4 or 5.\n", argc);
        exit(EXIT_FAILURE); 
    }
}

void printOptions(Options opts) {
    printHeader("Simulation Options");
    printf("\tInput File: %s\n", opts.filename);
    printf("\tNumber of frames: %d\n", opts.nFrames);
    printf("\tReplacement Algorithm: %s\n", opts.algo);
    printf("\tDepreciation Mode: %d\n", opts.debug_mode);
}

Simulator *simInit(Options opts) {
    Simulator *sim;
    FILE *fp;
    int i = 0;
    unsigned addr;
    char rw;

    setDebugMode(opts.debug_mode);

    sim = (Simulator *) malloc(sizeof(Simulator));
    memCheck(sim);

    fp = (FILE *) fopen(opts.filename, "r");
    fileCheck(fp, opts.filename);
    while(fscanf(fp, "%x %c ", &addr, &rw) == 2) {
        if (i >= MAX_MEM_ACCESS) {
            printf("ERROR: exceeded maximum number of rows in the log file.\n");
            exit(EXIT_FAILURE);
        }
        sim->memAccesses[i].addr = addr;
        sim->memAccesses[i].rw = rw;
        assert(rw == 'R' || rw == 'W');
        i++;
    }
    fclose(fp);

    sim->numMemAccesses = i;

    sim->mem = memInit(opts.algo, opts.nFrames);

    return sim;
}

void runSim(Simulator *sim) {
    int i;

    DEBUG printFrames(sim->mem);

    for (i = 0; i < sim->numMemAccesses; i++) {
        DEBUG printf("\n\n\n\t\tAccess %d / %d\n\n\t- %x %c\n",
                        i + 1, 
                        sim->numMemAccesses,
                        sim->memAccesses[i].addr,
                        sim->memAccesses[i].rw);
        DEBUG printf("\t- Written Pages: %d\n\t- Pages read: %d\n\n",
                     memStats(sim->mem).writes_to_disk,
                     memStats(sim->mem).page_faults);

        memClockInturrupt(sim->mem);
        accessMem(sim->mem, sim->memAccesses[i].addr, sim->memAccesses[i].rw);

        DEBUG printFrames(sim->mem);
        DEBUG getchar();
    }
}

void printSimResult(Simulator *sim) {
    Statistics stats = memStats(sim->mem);

    printHeader("Results");
    printf("\tTotal number of accesses to memory: %d\n\t Written Pages: %d\n\tPages read: %d\n",
        sim->numMemAccesses,
        stats.writes_to_disk, 
        stats.page_faults);
}

void simResult(Simulator *sim, int *writes_to_disk, int *page_faults) {
    Statistics stats = memStats(sim->mem);

    *writes_to_disk = stats.writes_to_disk;
    *page_faults = stats.page_faults;    
}

void freeSim(Simulator *sim) {
    freeMem(sim->mem);
    free(sim);
}
