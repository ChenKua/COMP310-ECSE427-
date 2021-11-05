//    Final Project COMP 310
//    Your Name: Kua Chen
//    Your McGill ID: 260856888
//
//    DISK_driver_problem4.h
//


#ifndef DISK_driver_problem4_h
#define DISK_driver_problem4_h

// For redirection purpose. For each open file.
char *pipeBuffers[MAX_OPENFILES];

// Correct current_location after readCommand
int correctLocation(int index);

// Malloc for buffer
// index is the index for active file table
void initalizeBuffer(int index);

// Clear the buffer when close_EXAM
void clearBuffer(int index);

#endif /* DISK_driver_problem4_h */