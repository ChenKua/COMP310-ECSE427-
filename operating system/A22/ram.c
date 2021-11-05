#include <stdlib.h>
#include <stdlib.h>
#include <string.h>
#include <stdio.h>

#include "pcb.h"

char *ram[1000];


//int index = 0;
int firstAvailable()
{   
    
    for (int i = 0; i < 1000; i++)
    {
        if (!ram[i])
        {
            return i;
        }
    }
    return -1;
}

void addToRAM(FILE *p, int *start, int *end){
    
    int index = firstAvailable();
    //printf("index: %d\n",index); 
    *start = index;
    
    if(index == -1){
        printf("ERROR: Not enough RAM to add program.\n");
        return;
    }

    
    char *line = (char *)malloc(1000 * sizeof(char));
    //printf("34\n"); 

    while(fgets(line,999,p))
    {   

        if(index>999){
            printf("ERROR: Not enough RAM to add program.\n");
            free(line);
            line = NULL;
            index = -1;
            *start = index;
            return;
        }

        // if (!strcmp(line, "\n"))
        // {   
        //     continue;           
        // }
        // if (!strcmp(line, " "))
        // {   
        //     continue;              
        // }
        // if (!strcmp(line, ""))
        // {   
        //     continue;              
        // }
        // if (!strcmp(line, "\r"))
        // {   
        //     continue;             
        // }
        
        ram[index] = strdup(line);
        
        // size_t line_size = (strlen(line) + 1) * sizeof(char);
        // ram[index] = (char *)malloc(line_size);
        // memcpy(ram[index], line, line_size);
        index++;
    }
    index--;
     
    *end = index;
     
    //printf("index2: %d\n",index); 
    //return;
}

void clearRAM(){
    for (int i = 0; i < 1000; i++)
    {   
        free(ram[i]);
        ram[i] = NULL;
    }
    return;
}

void deletePCB(PCB* pcb){
    int start = pcb->start;
    int end = pcb->end;
    //printf("start: %d\n",start);
    //printf("end: %d\n",end);

    while(start<=end){
         
        ram[start] = NULL;
        start ++;
        //printf("index %d()\n",start);
    }
    //printf("pcb delete\n");
    free(pcb);
    return;
}
