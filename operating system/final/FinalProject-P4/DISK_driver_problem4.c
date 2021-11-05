//    Final Project COMP 310
//    Your Name: Kua Chen
//    Your McGill ID: 260856888
//
//    You need to fill in this file for the fourth random problem



//	  Fill in the name of the command that you are implementing here 
//    ------------------>COMMAND NAME<-----------------------------

#include "DISK_driver.h"
#include "DISK_driver_problem1.h"
#include "DISK_driver_problem2.h"
#include "DISK_driver_problem3.h"
#include "DISK_driver_problem4.h"

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <sys/types.h>
#include <sys/stat.h>

// Buffers for redirection
char *pipeBuffers[MAX_OPENFILES];

/*
Correct current_location.
Due to the reqiurement, Seek 0 -4 will set the current_location to Block 2, offset 1.
However, after read the entire file (has 4 blocks), the current_location is at 4. (Next avaliable block pointer)
Seek 0 -4, is go back to 1 previous block and go to the character index with 1. => Block 3, char 1
Therefore, it semms that after read the entire file, the current_location should be set the last block pointer, 
     instead of the next avalible block pointer.
Therefore, after read command it done, just do current_location-- so that now it will point to the last block pointer, i.e: 3.
*/
int correctLocation(int index) {
    fat[index].current_location --;
    return 0;
}

// Malloc buffer
// Input index is the index for active file table
void initalizeBuffer(int index) {
    pipeBuffers[index] = (char*)malloc(sizeof(1000));
}

// Free the block when close the FAT
void clearBuffer(int index){
    pipeBuffers[index] = NULL;
    free(pipeBuffers[index]);
}



