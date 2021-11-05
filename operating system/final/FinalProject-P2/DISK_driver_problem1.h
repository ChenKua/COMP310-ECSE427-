//    Final Project COMP 310
//    Your Name:
//    Your McGill ID: 
//
//    DISK_driver_problem1.h
//


#ifndef DISK_driver_problem1_h
#define DISK_driver_problem1_h

// Check if the file is opened
int findFileInActiveTable(int fileIndex);

// Open file
int openFile_EXAM(char *index, char *name);

// Reset current_location
void resetCurrentLocation(int index);

#endif /* DISK_driver_problem1_h */