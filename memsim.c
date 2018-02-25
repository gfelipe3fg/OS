#include <stdio.h>
#include <stdlib.h>
#include "memsim.h"

void memsim(FILE *file, int numOfFrames, char *algo, char *dQ){

    char F[10000];
    unsigned addr;
    char rw;
    int i, YN =1, f, l;

    for(i = 0; i <numOfFrames; i++)
    {
        F[i] = -1;
    }
    //printf("here");
    if(strcmp(algo, "FIFO") ==0)
    {
                for (i = 0; i < sizeof(numOfFrames); i++) 
                {
                   F[i] = -1;
                }
                while (fscanf(file, "%x %c", &addr, &rw) != EOF) {
                    l = sizeof(addr);
                    printf("yo");
                    FIFO(addr, F, l, numOfFrames);
                }
                fclose(file);
    
//                case 2:
//                    for (i = 0; i < f; i++) {
//                        F[i] = -1;
//                    }
//                    LRU(s, F, l, f);
//                    break;
//            case 3:
//                for(i=0;i<f;i++)
//                {
//                    F[i]=-1;
//                }
//
//                opt(s,F,l,f);
//                break;
    }
}
