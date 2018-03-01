#include "memory.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <limits.h>


PageTable * mem_init(int nFrames,char *algo){
    PageTable *mem = (PageTable *) malloc(sizeof(PageTable));
    if(mem == NULL){
        printf("Error allocating memory\n");
        exit(EXIT_FAILURE);
    }
    if(strcmp(algo,"FIFO")==0 || strcmp(algo,"fifo")==0){
        mem->algo = FIFO;
    }else if(strcmp(algo,"LRU")==0 || strcmp(algo,"lru")==0){
        mem->algo = LRU;
    }else if(strcmp(algo,"VMS")==0 || strcmp(algo,"vms")==0){
        mem->algo = VMS;
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
    queue * q= createQueue(MAX_ACCESSES);
    mem->m_queue = q;
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
            enqueue(mem->m_queue,mem->page_frames[i]);
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

PageFrame fifo(PageTable * mem){
    return dequeue(mem->m_queue);
}
void removePage(PageTable *mem, TraceEntry entry){
    PageFrame removed;
    if(mem->algo == FIFO)
       removed = fifo(mem);
    mem->page_frames[removed.frame_number].addr = -1;
    mem->numAvalFrames++;
    loadPage(mem,entry);
}

void accessMem(PageTable * mem, TraceEntry entry){
    int frame = findPage(entry.addr,mem);
    if(frame == -1){ // Frame not in memory,load it
       if(mem->numAvalFrames != 0) // It has available frames
        {
            loadPage(mem,entry);
        }
       else{ // Not enough space, algorithm must kick in to choose best method
       //     printf("OUT OF SPACE\n");
       //     printQueue(mem->m_queue);
            removePage(mem,entry);
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

void printQueue(queue *q){
    int i;
    printf("\n\nState of the queue:\n");
    for(i =0; i < q->size; i++){
        printf("[%08x]->",q->array[i].addr);
    }
    printf("head\n");
}

void freeMem(PageTable *mem){
    free(mem->page_frames);
    free(mem);
}

queue * createQueue(unsigned capacity){
    queue * q = (queue *) malloc (sizeof(queue));
    
    q->capacity = capacity;
    q->front_idx=q->size =0;
    q->rear_idx = capacity-1;
    q->array = (PageFrame *) malloc(capacity * sizeof(PageFrame));
    q->front = q->array[0];
    q->rear = q->array[capacity-1];
    return q;
}


void enqueue(queue* q,PageFrame item){
    if(isFull(q))
        return;
    q->rear_idx = (q->rear_idx +1)%q->capacity;
    q->array[q->rear_idx] = item;
    q->rear = q->array[q->rear_idx];
    q->size++;
}

PageFrame dequeue(queue *q){
    PageFrame deq = q->array[q->front_idx];
    q->front_idx = (q->front_idx+1) % q->capacity;
    q->size--;
    q->front = q->array[q->front_idx];
    return deq;
}

int isEmpty(queue *q){
    return (q->size == 0);
}

int isFull(queue *q){
    return (q->size == q->capacity);
}

int front(queue *q){
    if(isEmpty(q))
        return INT_MIN;
    return q->front_idx;
}
int rear(queue *q){
    if(isEmpty(q))
        return INT_MIN;
    return q->rear_idx;
}
