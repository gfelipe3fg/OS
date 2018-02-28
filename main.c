#include "simulator.h"
#include <stdlib.h>
#include <stdio.h>
#include <string.h>

int main(int argc, char const *argv[]){
    if(argc != 5){
        printf("The right format is: ./memsim <tracefile> <nframes> <lru|fifo|vms> <debug|quiet>\n");
        exit(EXIT_FAILURE);
    }else{
    Options ops;
    strcpy(ops.filename,argv[1]);
    ops.frames_input = atoi(argv[2]);
    strcpy(ops.method,argv[3]);
    if(strcmp(argv[4],"debug") == 0)    
        ops.debug_mode = 1;
    else
        ops.debug_mode = 0;
    
    runSim(ops);
    }
    return 0;
}
    
