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
        mem->page_frames[i].accessed_w = 0;
    }

    return mem;
}

void loadPage(PageTable * mem, TraceEntry entry){
    int i;
    for(i = 0; i < mem->nframes; i++){
        if(mem->page_frames[i].addr == -1) { // Found an empty slot
            mem->page_frames[i].addr = entry.addr;
            mem->page_frames[i].rw = (entry.rw == 'W') ? 1 : 0;
            mem->page_frames[i].pos = 0;
            mem->page_frames[i].frame_number = i;
            mem->page_frames[i].accessed_w = mem->page_frames[i].rw;
            break;
        }
    }
    mem->numAvalFrames--;
}

int findPage(int addr, PageTable * mem){
    int i;
    // Look for page 
    for(i =0; i < mem->nframes; i++){
        if(mem->page_frames[i].addr == addr)
            return mem->page_frames[i].frame_number;
    }
    //Page not on table. table is full
    return -1;

}


void accessMem(PageTable * mem, TraceEntry entry){
    int frame = findPage(entry.addr,mem);
    if(frame == -1){ // Frame not in memory,load it
       if(mem->numAvalFrames != 0) // It has available frames
        {
            printf("Went to load a page\n");
            loadPage(mem,entry);
        }
       else{ // Not enough space, algorithm must kick in to choose best method
            printf("OUT OF SPACE\n");
            exit(EXIT_FAILURE);
       }
    }else{           // Frame in memory, just retreive it
        if(entry.rw == 'W'){
            mem->page_frames[frame].rw = 1;
            mem->page_frames[frame].accessed_w=1;
        }
    }
}

void print_memory(PageTable *mem){
    int i = 0;
    printf("+--------------------------------------------------------------\n");
    printf("+\tFN\t\tVPN\t\tRW\t\tPOS\n");
    for(;i<mem->nframes;i++){
        printf("+\t%d\t\t%08x\t%d\t\t%d\n",
                mem->page_frames[i].frame_number,
                mem->page_frames[i].addr,
                mem->page_frames[i].rw,
                mem->page_frames[i].pos);
    }
    printf("+--------------------------------------------------------------\n");

}
void freeMem(PageTable *mem){
    free(mem->page_frames);
    free(mem);
}
