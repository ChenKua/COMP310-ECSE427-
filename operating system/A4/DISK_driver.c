// Programmer: <Kua Chen>, <260856888> 
// Date: <Mar 30, 2021> 
// Purpose: <This file is a simulation of disk driver functionality.>

#include<stdio.h>
#include<stdlib.h>
#include<string.h>
#include"DISK_driver.h"
#include <dirent.h>
#include <errno.h>
#include <unistd.h>

/*
The active file table contains all the system wide open file pointers, which is 5 maximum. 
This is a private structure but global within DISK_driver.c.
*/
FILE *active_file_table[5];

/*
Another data structure to remember which active_file_table entries belong to which fat[].
*/
int active_FAT[5];

/*
The block buffer is used by the read_block() function to bring in blocks from disk. 
This is a private structure but global within DISK_driver.c.
*/
char *block_buffer;

/*
File name: PARTITION/XXXX
*/
char fileName[1000];

/*
Size of struct partition
*/
int partition_size = sizeof(struct PARTITION);

/*
Size of struct FAT
*/
int FAT_size = sizeof(struct FAT);

/*
Point to the root partition file
*/
FILE *originalPartition;

/*
Initialize all global data structure and variables to zero or null.
*/
void initIO(){
    printf("initialize IO!\n");

    Partition.total_blocks = 0; 
    Partition.block_size = 0; 

    for (int x = 0; x < 20; x++) {
        strcpy(fat[x].filename, "_NONE_");
        fat[x].file_length = 0;
        for(int i = 0; i < 10; i++){
            fat[x].blockPtrs[i] = -1;
        }

        fat[x].current_location = -1;
    }

    block_buffer = NULL; 

    for(int x = 0; x < 5; x++){
        active_file_table[x] = NULL;
        active_FAT[x] = -1;
    } 
    printf("IO initilized!\n");
}

/*
Create & format partition. 
Called from your mount() function in the  interpreter, associated to the scripting mount command.
*/
int partition(char *name, int blocksize, int totalblocks){
    DIR* dir = opendir("PARTITION");
    // if directory exists, do nothing
    if (dir) {

    //if directory does not exist, create it
    } else if (ENOENT == errno) {
        system("mkdir PARTITION");
    }
    
    strcpy(fileName, "PARTITION/"); 
    strcat(fileName, name); 
    char* route;
    route = strdup(fileName);
    printf("%s is stored in address: %s\n", name, route);
    FILE *fp = fopen(route, "wr");

    //write PARTITION, fat[20] into the file
    if (fp) {
        Partition.block_size   = blocksize;
        Partition.total_blocks = totalblocks;
        fwrite(&Partition, sizeof(struct PARTITION), 1, fp);

        for (int x = 0; x < 20; x++) { 
            fwrite(&fat[x], sizeof(struct FAT), 1, fp);
        }

        // Insert 0s
        for(int x = 0; x < blocksize * totalblocks; x++){
            fprintf(fp, "0"); 
        }
    } else {
        printf("Erroe in creating FILE: %s\n", name);
        return 0;
    }

    fclose(fp);
    return 1;
}

/*
Update partition file on the disk.
What change happen to fat[20] is in RAM, we need to wrtie fat[20] into disk.
*/
void updatePartition(){
    // File originalPartition is a variable visible inside this class
    // It is initilized in mountFS();
    if(originalPartition){
        fseek(originalPartition, 0, SEEK_SET);
        fwrite(&Partition, sizeof(struct PARTITION), 1, originalPartition);
        for (int x = 0; x < 20; x++) {
            fwrite(&fat[x], sizeof(struct FAT), 1, originalPartition);
        }
    }
}

/*
Clear the active file table
This method is called inside the mount
*/
void clearActiveFAT(){
    for (int i = 0; i < 5; i++) {
        if (active_file_table[i]) {
            fclose(active_file_table[i]);
            active_file_table[i] = NULL;
        }
        active_FAT[i]            = -1;
    }
}

/*
Load FAT & create buffer_block. 
Called from mount() function in the interpreter, associated to the scripting mount command.
*/
int mountFS(char *name){
    clearActiveFAT();

    strcpy(fileName, "PARTITION/"); 
    strcat(fileName, name);
    char* route;
    route             = strdup(fileName);
    originalPartition = fopen(route, "rb+");

    //Load PARTITION and fat[20]
    if (originalPartition) {
        fread(&Partition, sizeof(Partition), 1, originalPartition);
        for (int x = 0; x < 20; x++){
            fread(&fat[x], sizeof(struct FAT), 1, originalPartition);
            fat[x].current_location = 0;    //reset to 0
        }
    } else {
        printf("Error in opening file: %s. No such file\n", name);
        return 0;
    }

    block_buffer = (char*) malloc(Partition.block_size * sizeof(char));
    
    printf("Partition: %s Loading succeeded. Partition: Total Blocks:%d, Block size:%d\n" ,name ,Partition.total_blocks, Partition.block_size);
    return 1;
}

/*
Print the content of active file table
*/
void printFAT(){
    printf("Active File Table \n");
    printf("{%d, ", active_FAT[0]);
    printf("%d, ", active_FAT[1]);
    printf("%d, ", active_FAT[2]);
    printf("%d, ", active_FAT[3]);
    printf("%d}\n", active_FAT[4]);
}

/*
To set an entry in the active file table.
Need to fopen() the partition and then fseek() to the block.
index is fat[index]
*/
int addEntryToActive_File_Table(int index){

    // If directly store FILE *partion into the array, somehow, writeBlock() cannot successfully
    // write the data into file.
    // Have to use double pointer: fdopen(dup(fileno(partition)), "r+").
    FILE *partition      = fopen(fileName, "r+"); 
    FILE *buffer         = fdopen(dup(fileno(partition)), "r+");
    
    // Byte location of the starting block of the data region;
    int current_location = fat[index].current_location;
    int block_number     = fat[index].blockPtrs[current_location];
    int location         = partition_size + FAT_size * 20 + block_number * Partition.block_size;
    // Set the location to the block
    fseek(partition, location, SEEK_SET);
    
    // Check if the fat[index] is already in the active file table.
    int alreadyIn        = 0;
    for (int i = 0; i < 5; i++) {
        if (active_FAT[i] == index) {
            return 0;
        }
    }

    fclose(partition);

    // if the fat[index] is not in the active file table
    // add element
    int index_ATF = -1;
    for (int x = 0; x < 5; x++) {
        if (active_file_table[x] == NULL){
            index_ATF = x;
            active_file_table[x] = buffer;
            active_FAT[x]        = index;
            return 0;
        }
    }

    // No available cells in the active file table
    if (index_ATF == -1) {
        printf("<ERROR>: No available cells in the active file table\n");
        return -1;
    }
}

/*
This searches for the string name argument in the filename fields of fat[20].
Find filename or creates file if it does not exist, returns file’s FAT index. 
Called from the scripting read() and write() commands in the interpreter.
*/
int openfile(char *name){
    //search for the name among fat[20]
    int index = -1;
    for (int x = 0 ;x < 20; x++) {
        if (strcmp(name, fat[x].filename) == 0) {
            index = x;
            break;
        }
    }

    //Finds the file.
    //An available cell in the active file table is made to 
    //point to the first block of the
    //file and the index of the FAT cell is returned.
    if (index != -1) {
        int err = addEntryToActive_File_Table(index);
        if (err == -1) {
            return -1;
        } else {
            return index;
        }
        
    //Can't find the file.
    //Creates a new entry in the FAT table and returns the FAT index to that new entry
    } else {
        int available = -1;
        //Create a new entry
        for (int x = 0; x < 20; x++) { 
            if (strcmp(fat[x].filename,"_NONE_") == 0) {
                strcpy(fat[x].filename, name); 
                fat[x].current_location = 0;
                // allocate 10 contiguous block to the file.
                // if there are not enough space, return error
                for (int i = 0; i < 10; i++) {
                    if (x * 10 + i > Partition.total_blocks) {
                        printf("<ERROR>: No enough data region for the new file\n");
                        return -1;
                    }
                    fat[x].blockPtrs[i] = x * 10 + i;
                }
                addEntryToActive_File_Table(x);
                updatePartition();
                return x;
            }
        }   
    }
    printf("<ERRPR>: FAT is full\n");
    return -1;
}

/*
Using the file FAT index number, load buffer with data from current_location. 
Return block data as string from block_buffer.
The pdf says return type can be char* and int. So I choose char* and error can be detected by checking
the content of the returned char*.
*/
char *readBlock(int file){
    if (file == -1 || fat[file].file_length == 0){
        printf("<ERROR> Cannot read block. Index is -1 or file length is 0\n");
        return NULL;
    }
    
    // Check if the file is in the active file table.
    int index = -1;
    for (int x = 0; x < 5; x++) {
        if(active_FAT[x] != -1 && active_FAT[x] == file){
            index = x;
            break;
        }
    }

    if (index == -1) {
        printf("<ERROR> Cannot find the file in the active file table\n");
        return NULL;
    }
    
    FILE *current = active_file_table[index];
    if (!current) {
        printf("<ERROR> Cannot find the file in the active file table\n");
        return NULL;
    }
    
    // Find the block
    int current_location = fat[file].current_location;
    int block_number     = fat[file].blockPtrs[current_location];
    int location         = partition_size + FAT_size * 20 + block_number * Partition.block_size;

    // Current-location must point to a block and there are only max 10 blocks
    if (current_location >= 10) {
        printf("EOF\n");
        return NULL;
    }

    // Block number cannot be larger than or eqal to the total number of blocks
    // If so, there will be no data region allocated for this block
    if (block_number >= Partition.total_blocks) {
        printf("EOF\n");
        return NULL;
    }

    fseek(current, location, SEEK_SET);
    //Get the data from data region
    char tmp[Partition.block_size];
    for(int x = 0; x < Partition.block_size; x++){
        if(!feof(current)){
            char charData = fgetc(current);
            tmp[x]  = charData;
        }
    }
    strcpy(block_buffer,tmp);

    current_location++;
    fat[file].current_location = current_location; 
    updatePartition();
    return block_buffer;
}

/*
Return number of blocks needed for the data.
*/
int numberOfBlocks(char *data){
    int len    = strlen(data);
    int blocks = len / Partition.block_size;
    if (len % Partition.block_size != 0) blocks++;
    return blocks;
}

/*
Using the file FAT index number, write data to disk at current_location
*/
int writeBlock(int file, char *data){
    int len = strlen(data);
    
    if (file == -1 || len == 0) {
        printf("<ERROR>: Cannot read block. Index is -1 or data length is 0\n");
        return -1;
    }
    
    //Check if the file is in the active file table
    //Index of the file in active file table 
    int index  = -1;
    for (int x = 0; x < 5; x++) {
        if(active_FAT[x] != -1 && active_FAT[x] == file) {
            index = x;
            break;
        }
    }

    if (index == -1) {
        printf("<ERROR>: Cannot find the file in the active file table\n");
        return -1;      
    }

    // If directly store FILE *partion into the array, somehow, writeBlock() cannot successfully
    // write the data into file.
    // Have to use double pointer: fdopen(dup(fileno(partition)), "r+").
    FILE *openFile       = active_file_table[index];
    FILE *buffer         = fdopen(dup(fileno(openFile)), "w+");
    // find the current block
    int current_location = fat[file].current_location;
    int block_number     = fat[file].blockPtrs[current_location];

    // location of the block
    // Set the location to the current block
    int location     = partition_size + FAT_size * 20 + block_number * Partition.block_size;
    fseek(buffer, location, SEEK_SET);

    int charIndex    = 0;                          // index of the character in the data[]
    int blocksNeeded = numberOfBlocks(data);    // number of blocks needed for this character array
    int error        = 0;
    for (int i = 0; i < blocksNeeded; i++) {
        if (block_number >= Partition.total_blocks) {
            printf("<ERROR>: No enough data region for the file\n");
            error =  -1;
            break;
        }
        if (current_location >= 10){
            printf("<ERROR>: EOF\n");
            error = -1;
            break;
        }

        // Insert the character one by one.
        // Stop inserting when reach the block-size of the reach the end of the character array
        for (int a = 0; a < Partition.block_size; a++) {
            if (data[charIndex] != '\0') {
                //printf("<Write>: %c\n", data[charIndex]);
                fwrite(&data[charIndex], sizeof(char), 1, buffer);
                charIndex++;
            } else {
                break;
            }       
        }
        current_location++;
        fat[file].current_location = current_location;  
        block_number++;
    }

    fat[file].current_location     = current_location; 
    // update the file length.
    int length                     = fat[file].file_length; 
    // Notice that current-location can be reset to 0 when mount. But file-length may not be set to 0.
    if (length < current_location) {
        fat[file].file_length = current_location;
    }
    updatePartition();

    // Must close the file pointer after writing, otherwise the data region in the file will not be updated.
    fclose(buffer);     
    return error;
}