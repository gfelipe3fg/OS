#include <stdio.h>
#include <string.h>
#include <ctype.h>
#include <stdlib.h>

#ifndef MEMSIM_H
#define MEMSIM_H


void FIFO(unsigned ,char [],int,int);
//void LRU(unsigned , char, int, int);
void memsim(FILE *, int, char[], char[]);
//void opt(char [],char [],int,int);

#endif