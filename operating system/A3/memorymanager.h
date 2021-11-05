// Programmer: <Kua Chen>, <260856888> 
// Date: <Mar 17, 2021> 
// Purpose: <This file is the Modular programming techniques.>

#include <stdio.h>
#include"pcb.h"

/*
    Copy the file into backingstore and create PCB for this file.
*/
int launcher(FILE*p, char* filename);

/*
    Count total number of pages needed by the program.
    For example, if the program has L lines or less of code, 
    if L £ 4 the function returns 1. If 4 < L £ 8, it returns 2, etc.
*/
int countTotalPages(FILE *f);

/*
    Use a for loop to find the first empty frame.
    If one exists then return its index number, otherwise return -1.
*/
int findFrame();

/*
    The function loads the 4 lines of code from the page into the frame in ram[].
*/
void loadPage(int pageNum, FILE *f, int frameNum);

/*
    If there is no empty frame avaliable, this method will find a victim frame by
    using a random number generator to pick a frame number and
    check if this frame number is not beinig used by any other PCB.
    If this frame is not used by anyone, pick it.
    Else, iteratively increment the frame number (modulo-wise) until finding one
*/
int findVictim(PCB *p);

/*
The page table must be updated to reflect the changes. If a victim is invovled,
victim's table must be updated as well.
*/
int updatePageTable(PCB *p, int pageNumber, int frameNumber, int victimFrame);