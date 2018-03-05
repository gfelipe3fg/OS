#ifndef UTIL_H
#define UTIL_H
#include <limits.h>

// A node for the double linked list queue. This node contains a reference
// to the page/frame number of the frame, not the PageFrame object itself.
typedef struct QNode
{
    struct QNode *prev, *next;
    unsigned pageNumber;
}QNode;


// A double linked list based queue. Old elements are kept at the rear.
typedef struct queue_linked{
    unsigned count;  // Items used so far
    unsigned nframes; // Total number of items
    QNode *front,*rear;
}queue_linked;

// A hash tabel that contains pointer to nodes of the "queue_linked"
typedef struct hash{
    int capacity;  // Size fo the hash table
    QNode* *array; // Array of pointers to queue nodes.
}hash;


/*-------------------- Queue methods -----------------------------------------*/

// Creates a new node with the given page/frame number. This is not an addr.
QNode* newQNode( unsigned pageNumber );

// Creates a double linked list queue with the specified size.
queue_linked* createQueueLinked(int numberOfFrames);

// Add an element to the queue and inserts to the hash.
void enqueue_linked(queue_linked * queue, hash* hash, unsigned pageNumber);

// Removes element form queue and makes its reference on the has null.
int dequeue_linked(queue_linked * queue,hash* hash);

// Checks if the queue is full.
int isQueueFull( queue_linked* queue );

// Checks if the queue is empty.
int isQueueEmpty(queue_linked* queue );
/*----------------------------------------------------------------------------*/


// This function is called when a page with given 'pageNumber' is referenced
// from cache (or memory). There are two cases:
// 1. Frame is not there in memory, we bring it in memory and add to the front
//    of queue_linked
// 2. Frame is there in memory, we move the frame to front of queue_linked
void ref_page(queue_linked* queue, hash* hash, unsigned pageNumber );

// Creates a hash table with all elements set to null.
hash* createHash( int capacity );

// Convenient function to print the queue.
void printLRU(queue_linked *q);

// Convenient function to print the hash.
void printHash(hash * hash);


#endif
