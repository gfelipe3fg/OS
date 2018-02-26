#ifndef SIMULATOR_H_
#define SIMULATOR_H_ 

#define MAX_MEM_ACCESS 1000005
#define MAX_ARGV 301

typedef struct Simulator Simulator;

typedef struct Options {
    char filename[MAX_ARGV];
    char algo[MAX_ARGV];
    int nFrames;
    int debug_mode;
} Options;

/* Parses the arguments passed in the command line
and returns a struct representing the options. */
Options argsOptions(int argc, const char **argv);
/* Prints the options, just to check. */
void printOptions(Options opts);

/* Initializes the simulator with the chosen options, loading
the accesses of an input file. */
Simulator *simInit(Options opts);

/* Executes simulation. */
void runSim(Simulator *sim);

/* Prints results of simulation. */
void printSimResult(Simulator *sim);
void simResult(Simulator *sim, int *writes_to_disk, int *page_faults);

/* De-allocates resources used by the simulator. */
void freeSim(Simulator *sim);

#endif
