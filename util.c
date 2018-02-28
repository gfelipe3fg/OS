#include "util.h"

#include <stdlib.h>
#include <assert.h>

int _debug_mode;
int _quiet_mode;

unsigned lg2(unsigned x) {
    unsigned r = 0u;
    while(x > 1u) {
       x = x >> 1u;
       r++;
    }
    return r;
}

void memCheck(void *ptr) {
    if (!ptr) {
        printf("ERROR: failed to allocate memory.\n");
        exit(EXIT_FAILURE);
    }
}

void fileCheck(FILE *fp, char *filename) {
    if (!fp) {
        printf("ERROR: Could not open file '%s'.\n", filename);
        exit(EXIT_FAILURE);
    }
}

void setDebugMode(int debug_mode) {
    _debug_mode = debug_mode;
    _quiet_mode = debug_mode;
}

int getDebugMode(void) {
    return _debug_mode;
}

int getQuietMode(void){
    return  _quiet_mode;
}

/* Implementation of a queue. */

void init_queue(queue *q) {
    q->first = 0;
    q->last = MAX_QUEUE-1;
    q->count = 0;
}

int push(queue *q, int x) {
    if (q->count >= MAX_QUEUE)
    { return 0; }
    else {
            q->last = (q->last+1) % MAX_QUEUE;
            q->elem[ q->last ] = x;    
            q->count = q->count + 1;

            return 1;
    }
}

int pop(queue *q) {
    int x;

    if (q->count <= 0) { return 0; }
    else {
            x = q->elem[ q->first ];
            q->first = (q->first+1) % MAX_QUEUE;
            q->count = q->count - 1;
    }

    return(x);
}

int getRandElem(queue *q) {
    return q->elem[ q->first + (rand() % q->count) ];
}

int empty(queue *q) {
    if (q->count <= 0) return (1);
    else return (0);
}

void printQueue(queue *q) {
        int i;

        if (empty(q)) {
            printf("[]\n");
            return;
        }

        // printf("l: %d, f: %d, c: %d, e: [", q->elem[q->last], q->elem[q->first], q->count);
        printf("[");

        i=q->first; 
        
        while (i != q->last) {
                printf("%d, ",q->elem[i]);
                i = (i+1) % MAX_QUEUE;
        }

        printf("%d]\n",q->elem[i]);
}

/* Simple implementation of a list. */

list *listCreate(int key, int score) {
    list *l = (list *) malloc(sizeof(list));
    memCheck(l);

    l->key = key;
    l->score = score;
    l->tail = NULL;

    return l;
}

list *freeList(list *l) {
    if (l) {
        freeList(l->tail);
        free(l);
    }
    return NULL;
}

list *listAppend(list *l, int key, int score) {
    if (!l) {
        return listCreate(key, score);
    } else {
        l->tail = listAppend(l->tail, key, score);
        return l;
    }
}

list *listPrepend(list *l, int key, int score) {
    list *head = listCreate(key, score);
    head->tail = l;

    return head;
}

list *listLast(list *l) {
    while (l && l->tail) l = l->tail;
    return l;
}

list *listInsert(list *l, int key, int score) {
    if (!l) {
        return listCreate(key, score);
    } else {
        if (score <= l->score) {
            return listPrepend(l, key, score);
        } else {
            l->tail = listInsert(l->tail, key, score);
            return l;
        }
    }
}

list *listRemoveFirst(list *l, int *key, int *score) {
    if (!l) return NULL;
    else { 
        list *head = l->tail;
        *key = l->key;
        *score = l->score;
        free(l);
        return head;
    }
}

int listOrdered(list *l) {
    if (!l) return 1;
    else {
        if (!l->tail) return 1;
        else return (l->score <= l->tail->score) && listOrdered(l->tail);
    } 
}

int listContains(list *l, int key) {
    if (!l) return 0;
    else {
        if (l->key == key) return 1;
        else return listContains(l->tail, key);
    }
}

int listCount(list *l) {
    if (!l) return 0;
    else {
        return 1 + listCount(l->tail);
    }
}

void listPrint(list *l) {
    if (!l) printf("null\n");
    else {
        printf("%d {%d} -> ", l->key, l->score);
        listPrint(l->tail);
    }
}
