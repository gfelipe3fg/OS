#include "simulator.h"
#include "memory.h"
#include "util.h"

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <assert.h>

/********** Functions **********/
Options argsOptions(int argc, const char **argv) {
    if (argc == 4 || argc == 5) {
        Options opts;
        strcpy(opts.filename, argv[1]);
        opts.nFrames = atoi(argv[2]);
        strcpy(opts.algo, argv[3]);
        if (argc == 5) {
            if(strcmp(argv[4],"debug")==0)
                opts.debug_mode = 1;
            else
                opts.debug_mode = 0;
        }
        else opts.debug_mode = 0;
        return opts;
    } else {
        printf("ERROR: Incorrect number of Arguments (%d). Expected 4 or 5.\n", argc);
        exit(EXIT_FAILURE); 
    }
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
                     sim->mem->stats.writes_to_disk,
                     sim->mem->stats.page_faults);

        //memClockInturrupt(sim->mem);
        accessMem(sim->mem, sim->memAccesses[i].addr, sim->memAccesses[i].rw);

        DEBUG printFrames(sim->mem);
        DEBUG getchar();
    }
}

void printSimResult(Simulator *sim) {
    Statistics stats = sim->mem->stats;
    printf("---[ Results ]---\n");
    printf("\tTotal memory frames: %d\n",sim->mem->numPF);
    printf("\tEvents in trace: %d\n",sim->numMemAccesses);
    printf("\tTotal disk writes: %d\n",stats.writes_to_disk);
    printf("\tTotal disk reads: %d\n",stats.reads_to_disk);
}

void freeSim(Simulator *sim) {
    freeMem(sim->mem);
    free(sim);
}
