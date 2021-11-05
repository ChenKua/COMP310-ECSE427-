#include <stdlib.h>
#include <stdlib.h>
#include <string.h>
#include <stdio.h>

#include "pcb.h"


PCB* makePCB(int start, int end){
    PCB *pcb = (PCB *)malloc(sizeof(PCB));
    pcb->start = start;
    pcb->PC = start;
    pcb->end = end;
    return pcb;
}



