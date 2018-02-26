#ifndef UTIL_H_
#define UTIL_H_

#include <stdio.h>

#define TRUE 1
#define FALSE 0

/* Debug mode */

#define DEBUG if(getDebugMode())
#define QUIET if(getQuietMode())

/* Prints the name of the int and its value. */
#define printvar(name) (printf("%s = %d\n", #name, name))

/* Returns log of x in base 2. */
unsigned lg2(unsigned x);

/* Checks for allocation error, called after a malloc. */
void memCheck(void *ptr);

/* Checks whether a file can be opened. */
void fileCheck(FILE *fp, char *filename);

void printHeader(const char *str);

void setDebugMode(int debug_mode);
int getDebugMode(void);
int getQuietMode(void);

/* Queue of integers to aid in some algorithms. */
#define MAX_QUEUE 50000

typedef struct queue {
    int elem[MAX_QUEUE+1],
        first,
        last, 
        count;
} queue;

void init_queue(queue *q);
int push(queue *q, int x);
int pop(queue *q);
int empty(queue *q);
void printQueue(queue *q);
int getRandElem(queue *q);

/* List of integers ordered by a score. */
typedef struct list {
    int key, score;
    struct list *tail;
} list;

#define EMPTY_LIST NULL

list *listCreate(int key, int score);
list *freeList(list *l);

list *listAppend(list *l, int key, int score);
list *listPrepend(list *l, int key, int score);
list *listInsert(list *l, int key, int score);
list *listRemoveFirst(list *l, int *ket, int *score);

list *listLast(list *l);
int listOrdered(list *l);
int listContains(list *l, int key);
int listCount(list *l);
void listPrint(list *l);

#endif
