#include "pcb.h"
#include <stdio.h>
#ifndef RAM_H
#define RAM_H


char *ram[1000];
void addToRAM(FILE *p, int *start, int *end);
void deletePCB(PCB* pcb);
void clearRAM();

#endif