//    Final Project COMP 310
//    Your Name: Kua Chen
//    Your McGill ID: 260856888
//
//    You need to fill in this file for the second problem
//

#include "DISK_driver.h"
#include "DISK_driver_problem1.h"
#include "DISK_driver_problem2.h"

/*
Free memory of any buffers used during opening, reading, writing to a file.
It will also make available the cell of the active file table data structure you used for Problem 1
to be useable for other opened files.
Input index is the index in active file table.
*/
void close_EXAM(int index){
    for (int i = 0; i < currentPartition.block_size; i++) {
        blockBuffer[i] = '\0';
    }
    fclose(active_file_table[index]);
    active_file_table_owners[index] = -1;
}
