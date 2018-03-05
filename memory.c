#include "memory.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

/*
 * Returns 1 for processes starting with address 3, 0 otherwise.
 */
int get_process(unsigned addr){
    addr = addr >> 28;
    if(addr == 3)
        return 1;
    else
        return 0;
}

/*
 * Initializes all members of the PageTable. Sets the size of the PageTable.
 */
PageTable * mem_init(int nFrames,char *algo, Statistics * stats){
    PageTable *mem = (PageTable *) malloc(sizeof(PageTable));
    if(mem == NULL){
        printf("Error allocating memory for PageTable\n");
        exit(EXIT_FAILURE);
    }

    // Set the correct algorithm for this simulation
    if(strcmp(algo,"FIFO")==0 || strcmp(algo,"fifo")==0){
        mem->algo = FIFO;
    }else if(strcmp(algo,"LRU")==0 || strcmp(algo,"lru")==0){
        mem->algo = LRU;
    }else if(strcmp(algo,"VMS")==0 || strcmp(algo,"vms")==0){
        mem->algo = VMS;
    }

    // Set the number of total frames and empty frames
    mem->nframes = mem->numAvalFrames = nFrames;

    mem->page_frames = (PageFrame *)  malloc(mem->nframes *sizeof(PageFrame));
    if(mem->page_frames == NULL){
        printf("Error allocating page frames for memory\n");
        exit(EXIT_FAILURE);
    }

    mem->stats = stats;    // Sets the stats for results printing at the end

    // Create LRU data structures
    mem->lru_q = createQueueLinked(nFrames);
    mem->lru_h = createHash(nFrames);

    if(mem->algo == VMS){
        // Set fifo_q to be the process 1 queue with RSS = nFrames/2
        mem->fifo_q = createQueue(nFrames/2);
    }else {
        // Use fifo_q as the main queue for FIFO
        mem->fifo_q = createQueue(MAX_ACCESSES);
    }

    // This queue is only used for process 2 queue when VMS is running.
    // RSS = nFrames/2
    mem->fifo_q_2 = createQueue(nFrames/2);

    // Set global queues for VMS. Size set to RSS + 1 of the process.
    mem->global_dirty = createQueue(nFrames/2 +1);
    mem->global_clean = createQueue(nFrames/2 +1);

    // Initialize all pages of the PageTable to a default value of -1. (hex ff)
    int i;
    for(i = 0; i < mem->numAvalFrames ; i++){
        mem->page_frames[i].rw = 0;
        mem->page_frames[i].addr = -1;
        mem->page_frames[i].frame_number = i;
        mem->page_frames[i].accessed_w = 0;
    }
    return mem;
}


/*
 * Adds a new page to the PageTable. Uses the address provided by the trace after
 * shifting it 12 bits (a VPN is 20 bits, a trace address is 32 bits).
 */
void loadPage(PageTable * mem, TraceEntry entry){
    int i;
    for(i = 0; i < mem->nframes; i++){
        if(mem->page_frames[i].addr == -1) { // Found an empty slot
            mem->page_frames[i].addr = entry.addr >> 12; // Shift address and set it.
            mem->page_frames[i].rw = (entry.rw == 'W') ? 1 : 0; // Set RW
            mem->page_frames[i].frame_number = i;        // Set page number
            mem->page_frames[i].accessed_w = mem->page_frames[i].rw;

            // Add to the right queue depending on the algorithm
            if(mem->algo == FIFO)
                enqueue(mem->fifo_q,mem->page_frames[i]);
            if(mem->algo == LRU) {
                enqueue_linked(mem->lru_q, mem->lru_h, i);
            }

            // Adds to the right process queue. If that queue is empty, dequeue
            // it and add the removed element to the clean or dirty queue, then
            // try and re-enqueue the frame again into the process queue.
            if(mem->algo == VMS){
                if(get_process(entry.addr)){ // Address start with 3
                    enqueue_vms(mem->fifo_q_2,mem->global_dirty,
                                mem->global_clean,mem->page_frames[i]);
                }else{  // Address start with something not 3
                    enqueue_vms(mem->fifo_q,mem->global_dirty,mem->global_clean,
                                mem->page_frames[i]);
                }
            }

            // Everytime a page is loaded into memory (PageTable), count a read.
            // A page is not loaded unless is not in memory.
            mem->stats->reads_from_disk++;
            break;
        }
    }
    mem->numAvalFrames--; // One less availale frame now
}


/*
 * Tries to find a page with the given address in the PageTable. If address match
 * return the page/frame number, otherwise return -1 for not found.
 */
int findPage(int addr, PageTable * mem){
    int i;
    // Look for page 
    for(i =0; i < mem->nframes; i++){
        if(mem->page_frames[i].addr == (addr >> 12))
            return mem->page_frames[i].frame_number;
    }
    //Page not on table. table is full
    return -1;
}

PageFrame fifo(PageTable * mem){
    return dequeue(mem->fifo_q);
}

PageFrame lru(PageTable *mem){
    // Get page number of frame to be removed
    int frame_idx = dequeue_linked(mem->lru_q,mem->lru_h);
    return mem->page_frames[frame_idx]; // Return PageFrame at the given index
}

/*
 * This function removes a PageFrame using VMS. Possible cases are:
 *  1. The clean queue is not empty, take its first element.
 *  2. The clean queue is empty, the dirty queue is not. Take its first element.
 *  3. Both clean and dirty queue are full, take an element from the process queue.
 *
 *  The PageFrame returned is the page to be removed from memory.
 *  VMS should prioritize removal of frames in the global queues before removing
 *  from the process queue itself.
 */
PageFrame vms(PageTable *pTable,queue_dlinked *p_queue) {
    return first_out(p_queue,pTable->global_clean,pTable->global_dirty);
}


// Calls the right replacement polucty to determine which page to be removed.
void removePage(PageTable *mem, TraceEntry entry){
    PageFrame removed;
    if(mem->algo == FIFO)
        removed = fifo(mem);
    if(mem->algo == LRU)
        removed = lru(mem);

    // Pass the right process queue to VMS depending on the trace entry.
    if(mem->algo == VMS){
        if(get_process(entry.addr))
            removed = vms(mem,mem->fifo_q_2);
        else {
            removed = vms(mem, mem->fifo_q);
        }
    }

    // If page was dirty, increment count
    if (mem->page_frames[removed.frame_number].accessed_w == 1) {
        mem->stats->writes_to_disk++;
    }
    mem->page_frames[removed.frame_number].addr = -1; // Reset this frame
    mem->numAvalFrames++; // An empty slot was created.

    // Add trace to PageTable now that an empty slot is created.
    loadPage(mem,entry);
}

/*
 * Simulates accessing memory from a given trace. These are the possible cases:
 * 1. Page is in memory, update its RW access. If LRU is running, reference the
 * PageFrame in the queue to bring it to the front. If VMS is running then remove
 * the PageFrame from the global enqueues and re-enqueue it into the process queue.
 *
 * 2. Page is not in memory, there is space available: just load the PageFrame into
 * the queue.
 *
 * 3. Page is not in memory, there is no space available: remove a PageFrame using
 * the right policy and load the new PageFrame and the newly freed slot.
 */
void accessMem(PageTable * mem, TraceEntry entry){
    int frame = findPage(entry.addr,mem);

    if(frame == -1){ // Frame not in memory,load it
       if(mem->numAvalFrames != 0) // It has available frames
        {
            loadPage(mem,entry);
        }
       else{ // Not enough space, algorithm must kick in to choose best method
            removePage(mem,entry);
        }
    }else{           // Frame in memory
        if(mem->algo == VMS){ // It's in memory but might not be in process queue
            retrieveFromGlobal(mem,mem->page_frames[frame],entry);
        }
        if(mem->algo == LRU) { // Reference frame to bring it to the front
            ref_page(mem->lru_q, mem->lru_h, frame);
        }
        if(entry.rw == 'W'){ // Update RW access.
            mem->page_frames[frame].rw = 1;
            mem->page_frames[frame].accessed_w=1;
        }
    }
}


/*
 * Remove the first (oldest) page from clean queue if not empty, or dirty queue
 * if not empty, or process queue.
 */
PageFrame first_out(queue_dlinked * p_queue, queue_dlinked * clean,
                    queue_dlinked * dirty){
    if(!isEmpty(clean)){
        return dequeue(clean);
    }else if(!isEmpty(dirty)){
        return dequeue(dirty);
    }else{
        return dequeue(p_queue);
    }
}


/*
 * Read the trace file and load the array.
 */
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
    stats->num_mem_accesses = i; // Total reads from the file are the number of
                                // accesses that are going to happen.
}

// Create a default node for the double linked list based queue.
node* newNode(PageFrame item){
    node *temp= (node*) malloc(sizeof(node));
    temp->frame = item;
    temp->next = temp->prev= NULL;
    return temp;
}

// Create a default double linked list based queue.
queue_dlinked * createQueue(unsigned capacity){
    queue_dlinked * q = (queue_dlinked *) malloc (sizeof(queue_dlinked));
    q->size = 0;
    q->capacity=capacity;
    q->front = q->rear=NULL;
    return q;
}

// Enqueues a PageFrame into the double linked list base queue.
//
// IMPORTANT: this method is not bound by the capacity of the queue, use
// enqueue_vms to enqueue with bounds. FIFO works fine since the PageTable size
// and the size of the FIFO queue is the same, so when no space is left on the
// PageTable a dequeue will be issued for FIFO keeping the FIFO queue bounded at
// nframes.

void enqueue(queue_dlinked* q,PageFrame item){
    // Create a new LL node
    node *temp = newNode(item);

    // If queue is empty, then new node is front and rear both
    if (q->rear == NULL)
    {
        q->front = q->rear = temp;
        q->size++;
        return;
    }

    // Add the new node at the end of queue and change rear
    temp->prev = q->rear;
    q->rear->next = temp;
    q->rear = temp;
    q->size++;
}

// Removes a page frame from the double linked list based queue.
PageFrame dequeue(queue_dlinked *q){
    PageFrame f = {.accessed_w=0,.addr=-1,.frame_number=-1,.rw=0};
    if (q->front == NULL) {
        printf("Queue was empty!");
        q->size = 0;
        return f;
    }

    // Store previous front and move front one node ahead
    node *temp = q->front;
    PageFrame frame = temp->frame;
    q->front = q->front->next;

    // If front becomes NULL, then change rear also as NULL
    if (q->front == NULL)
        q->rear = NULL;
    q->size--;
    free(temp);
    return frame;
}

/*
 * Checks to see if a given page is in the process queue. If it is, reset its
 * RW access to match the "entry" RW. If page not in process queue, look for it
 * in the clean queue first. If found, remove it and added to the process queue.
 * If page not in clean queue, look for it in the dirty queue. If found remove it
 * and add it to the process queue. The PageFrame is assumed to be at one of these
 * three queues as this method is only called when a page hit occurs on the P
 * PageTable.
 */
void retrieveFromGlobal(PageTable * mem,PageFrame frame,TraceEntry entry){
    queue_dlinked * p_queue ; // The process queue to use
    if(get_process(entry.addr)){
        p_queue = mem->fifo_q_2;  // Belongs to process 2
    }else
        p_queue = mem->fifo_q;   // Belongs to process 1

    int flag = 0;    // Whether the page is on the process queue.
    node * temp=p_queue->front;

    for(;temp != NULL; temp = temp->next){
        if(temp->frame.addr == frame.addr){ // Frame found, update RW
            flag = 1;
            temp->frame.rw = entry.rw;
            temp->frame.accessed_w = (entry.rw == 'W') ? 1:0;
            break;
        }
    }
    if(!flag){  // Frame not found, look into clean and dirty queues.
        int foundClean = 0;
        for(temp=mem->global_clean->front;temp != NULL; temp = temp->next){
            if(temp->frame.addr == frame.addr){
                PageFrame toRemove = removeFromQueue(mem->global_clean,temp);
                foundClean = 1;
                // Re-enqueue into process the removed page.
                enqueue_vms(p_queue,mem->global_dirty,mem->global_clean,toRemove);
                break;
            }
        }
        if(!foundClean) { // Not in clean queue, has to be in this one.
            for (temp = mem->global_dirty->front;
                 temp != NULL; temp = temp->next) {
                    if (temp->frame.addr == frame.addr) {
                        PageFrame toRemove = removeFromQueue(mem->global_dirty,
                                                             temp);
                        // Re-enqueue into process the removed page.
                        enqueue_vms(p_queue,mem->global_dirty,mem->global_clean,
                                    toRemove);
                        break;
                    }
            }
        }
    }

}


/*
 * Removes the specified node from the double linked list queue. The node might
 * not be at the beginning (dequeueing position) of the queue. Pointers are
 * update as needed.
 */
PageFrame removeFromQueue(queue_dlinked * q, node * item){
    if(item == q->front) {
        return dequeue(q);
    }
    else if(item == q->rear){
        PageFrame frame = item->frame;
        q->front->next = NULL;
        q->rear = q->front;
        free(item);
        return frame;
    }else{
        PageFrame frame = item->frame;
        item->prev->next = item->next;
        item->next->prev = item->prev;
        free(item);
        return frame;
    }
}

/*
 * If the process queue is full, dequeue it. Removed element from process queue
 * is added to the clean or dirty queue depending on whether the page was dirty
 * or not. Finally, enqueue the passed PageFrame into the process queue (space
 * has been created now if it wasn't there before).
 */
void enqueue_vms(queue_dlinked* q,queue_dlinked* dirty, queue_dlinked *clean,
                 PageFrame item){
    if(isFull(q)) {
        PageFrame removed = dequeue(q);
        //printf("Removed %x, %d\n",removed.addr,removed.accessed_w);
        if(removed.accessed_w){  // Frame was dirty
            enqueue(dirty,removed);
        }
        else{
            enqueue(clean,removed);
        }
    }
    enqueue(q,item);

}


int isEmpty(queue_dlinked *q){
    return (q->front == NULL);
}

int isFull(queue_dlinked *q){
    return (q->size == q->capacity);
}

/*
 * Convenient method to print status of memory.
 */
void print_memory(PageTable *mem){
    int i = 0;
    printf("\nMemory:\n");
    printf("++\tFN\t\tVPN\t\tRW ++\n");
    for(;i<mem->nframes;i++){
        printf("->\t%d\t\t%05x\t%d  <-\n",
               mem->page_frames[i].frame_number,
               mem->page_frames[i].addr,
               mem->page_frames[i].rw);
    }
}

/*
 * Convenient method to print the resutls of the simulation.
 */
void print_results(PageTable *mem){
    printf("total memory frames: %d\n",mem->nframes);
    printf("events in trace: %d\n",mem->stats->num_mem_accesses);
    printf("total disk reads: %d\n",mem->stats->reads_from_disk);
    printf("total disk writes: %d\n",mem->stats->writes_to_disk);
}

/* Free all dynamic strucures */
void freeMem(PageTable *mem){
    free(mem->page_frames);
    free(mem->lru_h);
    free(mem->fifo_q);
    free(mem->lru_q);
    free(mem->fifo_q_2);
    free(mem->global_dirty);
    free(mem->global_clean);
    free(mem);
}

// Convenient method to print the double linked list queues used in VMS and FIFO.
void printQueue(queue_dlinked *q){
    node * temp = q->front;
    printf("Format: [VPN,FN]\n");
    for(;temp != NULL; temp=temp->next){
        printf("[%05x,%d]->",temp->frame.addr,temp->frame.frame_number);
    }

    if(q->front != NULL) {
        printf("end");
        printf("\nhead: [%05x,%d]", q->front->frame.addr,
               q->front->frame.frame_number);
        printf("tail: [%05x,%d]\n\n", q->rear->frame.addr,
               q->rear->frame.frame_number);
    }else{
        printf("\"EMPTY\"\n");
    }

}


