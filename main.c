#include<stdio.h>
#include <string.h>
#include <stdlib.h>
#include "memsim.h"

int main()
{
    int a, YN = 1, i, l, f;

    char trace[60];
    char F[10000], s[25], ch[20];
    unsigned addr;
    char rw;
    int r, w;
    FILE *fp = fopen("pract.trace", "r");

    printf("\n\nPlease enter the trace you will be using:");
    scanf("%s", trace);
    printf("\n\nPlease enter the algorithm of choice:");
    scanf("%s", s);
     printf("\n\nPlease indicate if this will be debug or quiet:");
    scanf("%s", ch);
    
    if (strcmp(trace, "bzip") == 0) {
        printf("bzip trace\n");
        if ((fp) == NULL) {
            printf("File not found\n");
            return 1;
        } else {
            while (fscanf(fp, "%x %c", &addr, &rw) != EOF) {
                f++;
                if (rw == 'R') {
                    r++;
                } else if (rw == 'W') {
                    w++;
                }
                memsim(fp, f, s, ch);
            }
        }
        printf("Frames: %d\n", f);
        printf("# of Reads %d\n", r);
        printf("# of Writes %d\n", w);
        fclose(fp);
    //    printf("1");
    //        while (fscanf(fp, "%x %c", &addr, &rw) != EOF) {
    //            printf("2");
    //            f++;
    //            if (strcmp((const char *) rw, "R") == 0) {
    //                r++;
    //                printf("%d", r);
    //            } else if (strcmp((const char *) rw, "W") == 0) {
    //                w++;
    //            }
    //        }
    } 
    else if (strcmp(trace, "gcc") == 0) {
        printf("gcc trace");
    } else if (strcmp(trace, "sixpack") == 0) {
        printf("sixpack trace");
    } else if (strcmp(trace, "swim") == 0) {
        printf("swim trace");
    } else {
        printf("Wrong trace");
    }
  
  return 0;
}