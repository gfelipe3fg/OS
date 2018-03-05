#include "memory.h"
#include <stdlib.h>
#include <stdio.h>
#include <string.h>

/*
 * Convenient function to print the current status, after each access, of the
 * PageTable and its correspondent queues. Prints relevant information to the
 * algorithm in use.
 */
void printStatus(PageTable * mem,int i){
    printf("\n*---------------    After access %d     --------------------------*\n",i+1);
    switch(mem->algo){
        case FIFO:
            printf("FIFO current status:\n");
            printQueue(mem->fifo_q);
            print_memory(mem);
            break;
        case LRU:
            printf("LRU queue current status:\n");
            printLRU(mem->lru_q);
            print_memory(mem);
            break;
        case VMS:
            printf("Process 1 queue: \n");
            printQueue(mem->fifo_q);
            printf("Process 2 queue: \n");
            printQueue(mem->fifo_q_2);
            printf("Clean queue: \n");
            printQueue(mem->global_clean);
            printf("Dirty queue: \n");
            printQueue(mem->global_dirty);
            print_memory(mem);
            break;
    }
    printf("*-----------------------------------------------------------------*\n\n");
}

/*
 * Loads all entries from the trace file  first, then initializes the PageTable
 * and runs one access to memory at a time. The PageTable in memory.c handles
 * all replacements and loads. When on debug mode, it will print the status
 * after each access has been made.
 */
void runSim(Options ops){
    TraceEntry entries[MAX_ACCESSES];      // Holds all entries from trace file
    Statistics stats;                      // Holds the stats use for results.

    loadEntries(entries,ops,&stats);       // Reads from trace file
    PageTable *mem = mem_init(ops.frames_input,ops.method,&stats);
    if(ops.debug_mode) print_memory(mem);
    int i;

    // Use one trace at a time to access memory
    for(i = 0; i < stats.num_mem_accesses;i++){
        accessMem(mem,entries[i]);
        if(ops.debug_mode) printStatus(mem,i);
    }
    print_results(mem);
    freeMem(mem);
}

/*
 * Takes input from the user on terminal and starts the simulator.
 */
int main(int argc, char const *argv[]){
    if(argc != 5){
        printf("The right format is: ./memsim <tracefile> <nframes> <lru|fifo|vms> <debug|quiet>\n");
        exit(EXIT_FAILURE);
    }else{
        Options ops;
        ops.frames_input = atoi(argv[2]);
        strcpy(ops.method,argv[3]);
        strcpy(ops.filename,argv[1]);
        if(strcmp(argv[4],"debug") == 0)
            ops.debug_mode = 1;
        else
            ops.debug_mode = 0;

        runSim(ops);
    }
    return 0;
}
    
