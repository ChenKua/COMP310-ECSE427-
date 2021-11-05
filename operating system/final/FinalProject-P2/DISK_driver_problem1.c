//    Final Project COMP 310
//    Your Name: Kua Chen
//    Your McGill ID: 260856888
//
//    You need to fill in this file for the first problem
//

#include "DISK_driver.h"
#include "DISK_driver_problem1.h"

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <sys/types.h>
#include <sys/stat.h>

/*
Check if the file is opned.
i.e. In the active file table.
*/
int findFileInActiveTable(int fileIndex){
    int FAT_index = active_file_table_owners[fileIndex];
    if (FAT_index != -1 && fat[FAT_index].filename!= NULL) {
        return 0;
    }
    return -1;
}

/*
Setup the data structures in the OS and locate the file in the partition.
Pointer to the file is initialized to point to the beginning of the file.
*/
int openFile_EXAM(char *index, char *name){

    int fileIndex = atoi(index);  // Translate string to int

    // Checking if the active file table [index] is avaliable
    if (fileIndex < 0 || fileIndex > MAX_OPENFILES) {
        printf("ERROR: Input file index out of bound.");
        return -1;
    }

    // Checking if there is a free block in the data section to create a new file
    if(currentPartition.last_free_block > currentPartition.total_blocks)
        return -1;

    // Lookup on the FAT
    // From codebase
    int FATindex = 0;
    for (; FATindex < MAX_FILES; FATindex++)
        if (fat[FATindex].filename == NULL || strcmp(fat[FATindex].filename, name) == 0)
            break;

    if (active_file_table_owners[fileIndex] == FATindex) {
        // the file is already opened, just reset current_location
        fat[FATindex].current_location = 0;
        return 0;
    }

    // this entry is occupied by other file. Return error
    if (active_file_table_owners[fileIndex] != -1) {
        printf("ERROR: Index number in use.");
        return -1;

    } else {

        active_file_table[fileIndex]        = fopen(filename, "r+");
        active_file_table_owners[fileIndex] = FATindex;

        if(FATindex == MAX_FILES && fat[MAX_FILES -1].filename != NULL){
            printf("ERROR: File DNE on the FAT and FAT is full.\n");
            fclose(active_file_table[fileIndex]);
            active_file_table[fileIndex] = NULL;
            active_file_table_owners[fileIndex] = -1;
        
            return -1;
        }
        else if(fat[FATindex].filename == NULL){
            // Make a new file
            fat[FATindex].filename = malloc(sizeof(500));
            strcpy(fat[FATindex].filename, name);
            fat[FATindex].current_location = 0;
            fat[FATindex].file_length = 0;
            return 0;
            // READ:
            // ALL Seeking is done when doing IO, since the file system is NOT contigous and files do not have a pre-allocated size.
            // Space in the data section will be allocated ON WRITE, so we are done here
        
            // Seeking to the data section
            //fseekToDataSection(active_file_table[activeFileIndex]);
            // Seeking to first block
        }
        else{
            // Open a current file
            // Data structures already initialized
            fat[FATindex].current_location = 0;
        }
        
    }


}

/*
When read_EXAM, current_location should be reset to 0 first.
Becuase after write_EXAM, current_location is pointing to the next avaliable block pointer.
*/
void resetCurrentLocation(int index){
    fat[index].current_location = 0;
}