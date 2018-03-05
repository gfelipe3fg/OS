#ifndef MEMORY_H
#define MEMORY_H

#include "util.h" // For access to LRU queues.

#define MAX_ARGV 301 // 300 characters max
#define MAX_ACCESSES 1000001 // There are one million traces on each file


// Defines the type of algorithm to run
typedef enum{
    FIFO,LRU,VMS
}Algo;

// An structure to hold information from the trace file.
typedef struct TraceEntry{
    char rw;  // Read or write
    unsigned addr; // 32 bits hex address
}TraceEntry;


// An structure to hold information about the results to be outputted.
typedef struct Statistics{
    int writes_to_disk;
    int reads_from_disk;
    int num_mem_accesses;   // Total accesses on trace file.
}Statistics;

// An structure to hold the options specified by the user on terminal
typedef struct Options{
    char filename[MAX_ARGV];
    char method[MAX_ARGV];
    int frames_input;
    int debug_mode;
}Options;

// An structure to hold an entry of the PageTable. This can be considered an
// individual page.
typedef struct PageFrame{
    char rw;                    // 0 for 'r', 1 for 'w'
    unsigned addr;              // 20 bit hex virtual address
    unsigned int frame_number;  // Frame number or index of the page
    unsigned int accessed_w;    // If it has ever been accessed to write
}PageFrame;

// A node used for the double linked list queue
typedef struct Node {
    PageFrame frame;  // A copy to the PageFrame on the table.
    struct Node *next;
    struct Node *prev;
}node;

// A queue based on a double linked list implementation. Old elements are kept
// at the rear.
typedef struct Queue{
    node *front,*rear;
    int size;
    unsigned capacity;
}queue_dlinked;

// A structure to hold the simulated page table
typedef struct PageTable{
    Algo algo;              // Algorithm used in this simulation.
    Statistics * stats;     // Statistics used for final result.
    PageFrame *page_frames; // Array of pages in the page table.
    int nframes;            // Total number of frames/pages this page table can hold.
    int numAvalFrames;       // Empty frames left
    queue_dlinked *fifo_q;   // For FIFO and any address not starting with 3 in VMS
    queue_dlinked *fifo_q_2; // For addresses starting with 3, not used in FIFO
    queue_dlinked *global_clean; // The global clean list used in VMS.
    queue_dlinked *global_dirty; // The dirty clean list used in VMS.
    queue_linked *lru_q;    // LRU queue based on linked list implementation.
    hash * lru_h;           // LRU hashmap based on linked list implementation.
}PageTable;

// Gets the process id (1 or 0) for the given address. Used for VMS.
int get_process(unsigned addr);

// Convenient method to print the PageTable status
void print_memory(PageTable *mem);

// Convenient method to print the results.
void print_results(PageTable *mem);

// Free memory and all of its members
void freeMem(PageTable *mem);

/* ----------------------  Double linked list queue methods ------------------*/

// Creates a new node for the double linked list queue.
node* newNode(PageFrame item);

// Creates a double linked queue with the specified size
queue_dlinked * createQueue(unsigned capacity);

// Whether the queue is full or not.
int isFull(queue_dlinked * q);

// Whether the queueu is empty or not.
int isEmpty(queue_dlinked * q);

// Add an element to the queue.
void enqueue(queue_dlinked * q,PageFrame frame_i);

// Remove element from the queue.
PageFrame dequeue(queue_dlinked *q);

// Tries to enqueue to "q", if "q" is full dequeue its element onto "dirty" or
// clean. Used for VMS only.
void enqueue_vms(queue_dlinked * q,queue_dlinked*dirty, queue_dlinked*clean,
                 PageFrame frame_i);

// Convenient function to print the status of the queue
void printQueue(queue_dlinked *q);

/*----------------------------------------------------------------------------*/


/* ------------- Methods for the PageTable -----------------------------------*/

// Initialized everything to default, sets the number of pages, the type of
// algorithm and the Statistics object used for results output.
PageTable* mem_init(int nFrames,char * algo,Statistics * stats);

// Loads entries from trace file onto array of traces.
void loadEntries(TraceEntry * entries,Options ops,Statistics* stats);

PageFrame fifo(PageTable *mem);  // Remove page using FIFO
PageFrame lru(PageTable *mem);   // Remove page using LRU
PageFrame vms(PageTable *mem,queue_dlinked *arr); // Remove page using VMS

// Removes page from PageTable using fifo(), lru() or vms()
void removePage(PageTable *mem, TraceEntry entry);

// Looks to see if the "addr" is already in the PageTable
int findPage(int addr,PageTable * mem);

// Adds a new page to the PageTable using the trace given
void loadPage(PageTable * mem, TraceEntry entry);

// Simulation of memory referencing using a given trace.
void accessMem(PageTable *mem, TraceEntry entry);
/*----------------------------------------------------------------------------*/


// Removes the first PageFrame (oldest) in the clean queue. If clean queue is empty
// tries to remove the first PageFrame from the dirty queue. If dirty queue is
// empty, removes first element from the given process queue (p_queue).
PageFrame first_out(queue_dlinked * p_queue, queue_dlinked * clean, queue_dlinked * dirty);

// Tries to find the given PageFrame in the clean or dirty queues. If found,
// remove it and add it to the correspondent process queue of the given entry.
// If it is in the process queue, and not on the clean or dirty queues, update
// it's RW access.
void retrieveFromGlobal(PageTable *mem,PageFrame frame,TraceEntry entry);

// Looks for given "node" in "array" and removes it. Node might not be in the
// rear (dequeue position) of the queue.
PageFrame removeFromQueue(queue_dlinked * array, node * node);

#endif

