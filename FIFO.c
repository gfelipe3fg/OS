#include <stdio.h>
#include "memsim.h"

void FIFO(unsigned addr,char F[],int l,int f)
{
    int i,j=0,k,flag=0,cnt=0;
    char s[25];
    
    printf("\n\tPAGE\t    FRAMES\t  FAULTS");
    for(i=0;i<l;i++)
    {
        for(k=0;k<f;k++)
        {
            if(F[k]== addr)
                flag=1;
        }

        if(flag==0)
        {
            printf("\n\t%x\t",s);
            F[j]= (char) addr;
            j++;

            for(k=0;k<f;k++)
            {
                printf("   %c",F[k]);
            }
            printf("\tPage-fault%d",cnt);
            cnt++;
        }

        else
        {
            flag=0;
            printf("\n\t%x\t",addr);
            for(k=0;k<f;k++)
            {
                printf("   %c",F[k]);
            }

            printf("\tNo page-fault");
        }
        if(j==f)
            j=0;
    }

}