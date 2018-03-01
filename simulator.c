#include <stdio.h>
#include <stdlib.h>
#include "simulator.h"
#include "memory.h"

void runSim(Options ops){
    PageTable *mem = mem_init(ops.frames_input,ops.method);
    TraceEntry entries[MAX_ACCESSES];
    Statistics stats;

    loadEntries(entries,ops,&stats);
    print_memory(mem);

    int i,garbage;
    for(i = 0; i < stats.num_mem_accesses;i++){
        accessMem(mem,entries[i]);
    }
    print_memory(mem);
}

void loadEntries(TraceEntry * entries,Options ops, Statistics * stats){
    int i = 0;
    FILE *fp = (FILE *) fopen (ops.filename,"r");
    if(fp == NULL){
        printf("Error openning file: %s\n",ops.filename);
        exit(EXIT_FAILURE);
    }
    while(fscanf(fp,"%x %c", &entries[i].addr,&entries[i].rw)==2){
        i++;
    }
    fclose(fp);
    stats->num_mem_accesses = i;
}
