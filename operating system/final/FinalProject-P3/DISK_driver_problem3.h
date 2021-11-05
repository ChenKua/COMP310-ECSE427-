//    Final Project COMP 310
//    Your Name: Kua Chen
//    Your McGill ID: 260856888
//
//    DISK_driver_problem3.h
//

#ifndef DISK_driver_problem3_h
#define DISK_driver_problem3_h

// Set file pointer
void seek_EXAM(int index, int offset);

// Write with offset
int writeBlock_EXAM(int file, char *data);

// Read with offset
char *read_EXAM(int file);


#endif /* DISK_driver_problem3_h */