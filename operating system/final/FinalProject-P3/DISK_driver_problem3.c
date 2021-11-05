//    Final Project COMP 310
//    Your Name: Kua Chen
//    Your McGill ID: 260856888
//
//    You need to fill in this file for the third problem
//

#include "DISK_driver.h"
#include "DISK_driver_problem1.h"
#include "DISK_driver_problem2.h"
#include "DISK_driver_problem3.h"

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <sys/types.h>
#include <sys/stat.h>
/*
The seek_EXAM command moves the file pointer by offset, starting from its current position.
The seek_EXAM does not move the file pointer outside of the file.
*/
void seek_EXAM(int index, int offset){

     // Get the file pointer
    FILE *filePtr = NULL;
    int i = 0;
    for (; i < MAX_OPENFILES; i++)
        if(active_file_table_owners[i] == index){
            filePtr = active_file_table[i];
            break;
        }
    if(i == MAX_OPENFILES)
        return; // File was not open before this call
    
    int current_location = fat[index].current_location; // Current location
    // fseekToBlock(filePtr, current_location);    // set the pointer to the current_location

    // E.g offset = 6, need one 5-size-block and 1 byte offset.
    int block_size        = currentPartition.block_size;    
    int offSetInBlocks    = offset / block_size;                        // block offset
    int offSetAfterBlcoks = offset - offSetInBlocks * block_size;       // byte offset
    int file_length       = fat[index].file_length;
    int lastBlockID       = fat[index].blockPtrs[file_length - 1];
    if (offSetAfterBlcoks >= 0) {
        // do nothing
    } else {
        // Take the modular. E.g -4 becomes -1 Block and 1 byte offset
        offSetInBlocks--;
        offSetAfterBlcoks = offSetAfterBlcoks + block_size;    
    }
    
    // Check if pointer is within the range
    // If the sum is less than 0.
    // Set to the start of the file.
    if (current_location + offSetInBlocks < 0) {
        printf("ERROR: Out of bounds. Stopped at start of file.\n");
        fseekToBlock(filePtr, fat[index].blockPtrs[0]); 
        fat[index].current_location = 0;
        fat[index].offset           = 0;
        return;
    }

    // If the sum is just 0 but byteOffset is still less 0
    // Set to the start of the file.
    if (current_location + offSetInBlocks == 0 && offSetAfterBlcoks < 0) {
        printf("ERROR: Out of bounds. Stopped at start of file.\n");
        fseekToBlock(filePtr, fat[index].blockPtrs[0]); 
        fat[index].current_location = 0;
        fat[index].offset           = 0;
        return;
    }

    // If the sum is large than the file length.
    // Set to the end of the file.
    if (current_location + offSetInBlocks >= file_length) {
        printf("ERROR: Out of bounds. Stopped at end of file.\n");
        fseekToBlock(filePtr, lastBlockID); 
        fseek(filePtr, block_size - 1 , SEEK_CUR);
        fat[index].current_location = file_length - 1;
        fat[index].offset           = block_size - 1;
        return;
    }

    fseekToBlock(filePtr, current_location + offSetInBlocks);
    fseek(filePtr, offSetAfterBlcoks, SEEK_CUR);

    fat[index].current_location = current_location + offSetInBlocks;
    fat[index].offset           = offSetAfterBlcoks;
    return; 
}

// file is id in fat[20]
int writeBlock_EXAM(int file, char *data){
    
    if(file < 0)
        return 0; // File open opreation probably failed before this call. We should never get here.
    
    if(fat[file].current_location >= MAX_BLOCKS)
        return 0; // No more blocks are available for the file
    
    int current_location = fat[file].current_location;
    int offset           = fat[file].offset;

    // Get the file pointer
    FILE *filePtr = NULL;
    int i = 0;
    for (; i < MAX_OPENFILES; i++)
        if(active_file_table_owners[i] == file){
            filePtr = active_file_table[i];
            break;
        }
    if(i == MAX_OPENFILES)
        return 1; // File was not open before this call
    
    // check if this needs a new block, or it is after seek_EXAM
    if (fat[file].current_location >=  fat[file].file_length) {
        fseekToNextFreeBlock(filePtr);
        offset = 0;
        fat[file].blockPtrs[fat[file].current_location] = currentPartition.last_free_block;
    } else {    // for case where it is after seek_EXAM or just after open_EXAM command
        fseekToBlock(filePtr, fat[file].blockPtrs[current_location]); 
        
        fseek(filePtr, offset, SEEK_CUR);
    }

    // Writing the block char by char into the buffer
    for (int i = offset; i < currentPartition.block_size; i++) {
        char c = *(data+i);
        if(c == 0)
            c = '0';
        fputc(c, filePtr);
    }
    
    // Updating the file's blockPtr array
    // fat[file].blockPtrs[fat[file].current_location] = currentPartition.last_free_block;
    
    currentPartition.last_free_block += 1;
    fat[file].current_location       += 1;

    // fat[file].file_length +=1;
    if (fat[file].current_location >= fat[file].file_length) {
        fat[file].file_length =fat[file].current_location;
    }

    fat[file].offset = 0;   // clear the offset     

    // If current_location does not exceed file length, it is overwriting the old data.
    // No need to put -1. 
    if(fat[file].current_location < MAX_BLOCKS && fat[file].current_location >= fat[file].file_length)
        fat[file].blockPtrs[fat[file].current_location] = -1;
    
    // Update the FAT on disk
    partitionFile = fopen(filename, "r+");
    updateFATOnDisk();
    fclose(partitionFile);
    
    return 1;
}

char *read_EXAM(int file){
    
    if(file < 0)
        return NULL; // File open opreation probably failed before this call. We should never get here.

    if(fat[file].current_location >= MAX_BLOCKS)
        return NULL;
    
    // Lookup the block to read from
    int blockToRead = fat[file].blockPtrs[fat[file].current_location];
    // Seeing the block is valid
    if(blockToRead == -1)
        return NULL;
        
    // Get the file pointer
    FILE *filePtr = NULL;
    int i = 0;
    for (; i < MAX_OPENFILES; i++)
        if(active_file_table_owners[i] == file){
                filePtr = active_file_table[i];
            break;
        }
    if(i == MAX_OPENFILES)
        return NULL; // File was not open before this call
    
    // Seek to the block
    fseekToBlock(filePtr, blockToRead);
    fseek(filePtr, fat[file].offset, SEEK_CUR);

    // Reading the block char by char into the buffer
    i = 0;
    for (; i < currentPartition.block_size - fat[file].offset; i++) {
        char c = fgetc(filePtr);
        if(c == '0'){
            blockBuffer[i] = '\0';
            break;
        }
        blockBuffer[i] = c;
    }
    blockBuffer[i] = '\0';
    fat[file].current_location += 1;
    fat[file].offset = 0;
    return blockBuffer;
}
