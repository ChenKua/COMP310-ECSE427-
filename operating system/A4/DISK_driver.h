// Programmer: <Kua Chen>, <260856888> 
// Date: <Mar 30, 2021> 
// Purpose: <This file is the Modular programming techniques.>

/*
The PARTITION structure records information about the format of the partition.
*/
struct PARTITION {
    int total_blocks;
    int block_size;
} Partition;

/*
The FAT contains information about all the files in a partition.
*/
struct FAT {
    char filename[1000];
    int file_length;
    int blockPtrs[10];
    int current_location;
} fat[20];

/*
Initialize all global data structure and variables to zero or null.
*/
void initIO();

/*
Create & format partition. 
*/
int partition(char *name, int blocksize, int totalblocks);

/*
Update partition file.
*/
int mountFS(char *name);

/*
Find entry in fat[20] and add to the active file table.
*/
int openfile(char *name);

/*
Read current location block.
*/
char *readBlock(int file);

/*
Write data into block startinf from current location.
*/
int writeBlock(int file, char *data);