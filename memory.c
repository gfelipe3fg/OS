#include "memory.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

PageTable * mem_init(int nFrames){
    PageTable *mem = (PageTable *) malloc(sizeof(PageTable));
    if(mem == NULL){
        printf("Error allocating memory\n");
        exit(EXIT_FAILURE);
    }
    
    mem->nframes = nFrames;
    mem->numAvalFrames = nFrames;
    mem->numUnavalFrames = 0; // All frames are available at start
    mem->page_frames = (PageFrame *)  malloc(mem->nframes *sizeof(PageFrame));
    
    if(mem->page_frames == NULL){
        printf("Error allocating page frames for memory\n");
        exit(EXIT_FAILURE);
    }
    
    int i;
    for(i = 0; i < mem->numAvalFrames ; i++){
        mem->page_frames[i].rw = 0;
        mem->page_frames[i].addr = -1;
        mem->page_frames[i].pos = 0;
        mem->page_frames[i].frame_number = i;
    }

    return mem;
}

void print_memory(PageTable *mem){
    int i = 0;
    printf("+------------------------------------------------------------------\n");
    printf("+\tFN\t\tVPN\t\tRW\t\tPOS\n");
    for(;i<mem->nframes;i++){
        printf("+\t%d\t\t%d\t\t%d\t\t%d\n",
                mem->page_frames[i].frame_number,
                mem->page_frames[i].addr,
                mem->page_frames[i].rw,
                mem->page_frames[i].pos);
    }
    printf("+------------------------------------------------------------------\n");

}
void freeMem(PageTable *mem){
    free(mem->page_frames);
    free(mem);
}
