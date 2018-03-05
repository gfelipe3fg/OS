#include "util.h"
#include <stdio.h>
#include <stdlib.h>

// Creates new node for the list.
QNode* newQNode(unsigned pageNumber){
    QNode * temp = (QNode *) malloc (sizeof(QNode));
    temp->pageNumber = pageNumber;
    temp->prev = temp->next = NULL;
    return temp;
}

// Creates a default queue. Total elements added (count) are set to 0.
queue_linked* createQueueLinked(int numberOfFrames){
    queue_linked* queue = (queue_linked*) malloc (sizeof(queue_linked));
    queue->count = 0;
    queue->front = queue->rear = NULL;
    queue->nframes = numberOfFrames;
    return queue;
}

hash* createHash(int capacity){
    hash *h = (hash *) malloc (sizeof(hash));
    h->capacity = capacity;
    h->array = (QNode**) malloc (h->capacity*sizeof(QNode*));
    int i;
    for(i =0;i < h->capacity;i++){
        h->array[i] = NULL;     // Sets all pointer to queue nodes to NULL.
    }
    return h;
}


int isQueueFull(queue_linked * queue){
    return queue->count == queue->nframes;
}

int isQueueEmpty(queue_linked * queue){
    return queue->rear == NULL;
}

int dequeue_linked(queue_linked * queue,hash * hash){
    if( isQueueEmpty( queue ) )
        return -1;

    // If this is the only node in list, then change front
    if (queue->front == queue->rear)
        queue->front = NULL;

    // Change rear and remove the previous rear
    QNode* temp = queue->rear;

    // PageNumber that will be removed from queue.
    int num = temp->pageNumber;

    //hash->array[ queue->rear->pageNumber ] = NULL;
    queue->rear = queue->rear->prev;

    // Update rear
    if (queue->rear)
        queue->rear->next = NULL;

    free( temp );

    // decrement the number of full frames by 1
    queue->count--;
    return num;
}

void enqueue_linked(queue_linked* queue, hash * hash, unsigned pageNumber){
    // If all frames are full, remove the page at the rear
    if ( isQueueFull( queue ) )
    {
        // remove page from hash
        hash->array[ queue->rear->pageNumber ] = NULL;
        dequeue_linked( queue,hash);
    }

    // Create a new node with given page number,
    // And add the new node to the front of queue_dlinked
    QNode* temp = newQNode( pageNumber );
    temp->next = queue->front;

    // If queue_dlinked is empty, change both front and rear pointers
    if ( isQueueEmpty( queue ) )
        queue->rear = queue->front = temp;
    else  // Else change the front
    {
        queue->front->prev = temp;
        queue->front = temp;
    }

    // Add page entry to hash also
    hash->array[ pageNumber ] = temp;

    // increment number of full frames
    queue->count++;
}

void ref_page(queue_linked * queue, hash* hash, unsigned pageNumber){
    QNode* reqPage = hash->array[ pageNumber ];

    // the page is not in cache, bring it
    if ( reqPage == NULL )
        enqueue_linked( queue, hash, pageNumber );

        // page is there and not at front, change pointer
    else if (reqPage != queue->front)
    {
        // Unlink rquested page from its current location
        // in queue_dlinked.
        reqPage->prev->next = reqPage->next;
        if (reqPage->next)
            reqPage->next->prev = reqPage->prev;

        // If the requested page is rear, then change rear
        // as this node will be moved to front
        if (reqPage == queue->rear)
        {
            queue->rear = reqPage->prev;
            queue->rear->next = NULL;
        }

        // Put the requested page before current front
        reqPage->next = queue->front;
        reqPage->prev = NULL;

        // Change prev of current front
        reqPage->next->prev = reqPage;

        // Change front to the requested page
        queue->front = reqPage;
    }
}


/*
 * Prints the status of the queue. Indicates the head and the tail for better
 * reference. If the queue is empty it notifies it.
 */
void printLRU(queue_linked *q){
    QNode* temp = q->front;
    if(temp == NULL)
        printf("Queue is empty\n");
    else {
        printf("");
        while (temp != NULL) {
            printf("[FN: %d]<-", temp->pageNumber);
            temp = temp->next;
        }
    }
    if(q->front != NULL) {
        printf("end\n");
        printf("head: %d, ", q->front->pageNumber);
        printf("tail: %d\n\n", q->rear->pageNumber);
    }
}

/*
 * Prints the status of the hash table. Not currently used in debug mode.
 */
void printHash(hash * hash){
    int i;
    for(i = 0; i < hash->capacity; i++){
        if(hash->array[i] == NULL)
            printf("idx: %d, value: NULL",i);
        else
            printf("idx: %d, value: %d\n",i,hash->array[i]->pageNumber);
    }
}


