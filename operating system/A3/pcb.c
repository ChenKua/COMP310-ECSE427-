#include <stdio.h>
#include <stdlib.h>
#include "pcb.h"


/*
PC Points to the start of the frame (i.e. frame * 4), it is initilized with -1
PageTable is also initilized with -1 for every element
*/
PCB* makePCB(int maxPage, FILE *f, char *filename){
    PCB* pcb = (PCB*)malloc(sizeof(PCB));
    pcb->PC         = -1;  
    pcb->pages_max  = maxPage;
    pcb->PC_offset  = 0;
    pcb->PC_page    = 0;
    for(int i = 0; i < 10; i++) {
        pcb->pageTable[i] = -1;
    }
    pcb->f    = f;
    pcb->name = filename;
    return pcb;
}


