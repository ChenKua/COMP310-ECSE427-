#include <stdio.h>
#include <stdlib.h>

#ifndef PCB_H
#define PCB_H
/*
PCB has 7 fields
PC        : Points to the current frame start
pageTable : Contains valid frames for some pages, if no valid frame, contains "-1"
PC_page   ; Points to the current page that the program is in
PC_offset : Offset is the line count from the start of the frame. Ranged from [0,4]
pages_max : Total number of pages of the file
f         : Points to the file
name      : Contains the name of the file
*/
typedef struct PCB
{
    int PC;
    int pageTable[10];
    int PC_page;
    int PC_offset;
    int pages_max;
    FILE *f;
    char *name;
}PCB;

/*
Passes 3 parameters (maxPage, f , name)
This method creates a PCB with fields set as this:
pages_max = maxPage
f         = f
name      = name
*/
PCB* makePCB(int maxPage, FILE *f, char *name);

#endif