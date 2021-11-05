// Programmer: <Kua Chen>, <260856888> 
// Date: <Mar 17, 2021> 
// Purpose: <This file is responsible with the exec command (not the run command), managing frames, and paging.>

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>
#include <sys/types.h>
#include"shell.h"
#include"pcb.h"
#include"ram.h"
#include"cpu.h"
#include"interpreter.h"
#include"kernel.h"

//
int count = 0;

// delete?
void clearCount(){
    count == 0;
}

/*
    Create new name for the file.
*/
char* createNewName(int count) {
    char* name = malloc(sizeof(char)*100);
    strcpy(name, "program");

    char number[100];
    sprintf(number, "%d", count); // translate int to char
    char *word = strdup(number);   
    strcat(name, word); 
    strcat(name, ".txt");
    return name;
}

/*  
    This method does the following things
    1. Copy the entire file into the Backing Store.
    2. Close the file pointer pointing to the original file.
    3. Open the file in the Backing Store.
    4. Our launch paging technique loads two pages of the program into RAM when it is first launched.    
*/
int launcher(FILE *p, char* filename) {    
    int error = 0;

    // Create new name for the file
    // Becasue now the shell can exec same progroam
    // Each file in the backingstore needs a unique name.
    char *fileName  = createNewName(count);  // e.g program1.txt 
    char path[1000] = "BackingStore/";
    strcat(path, fileName); //e.g. BackingStore/program1.txt
    char* route;
    route           = strdup(path);
    printf("File: %s is stored in: %s \n", filename, route);
    FILE *file      = fopen (path, "w");     //create file
    
    // fail in creating file
    if(!file){
        printf("Null exception in opening file: %s \n", route);
        return 0;
    }

    count++;

    //Copy file p into backstore file
    char line[1000];
    while(fgets(line,999,p)){
        fputs(line, file);
    }

    fclose(p);
    fclose(file);

    FILE *f    = fopen(path, "r");
    int result = myinit(f, filename);

    return result;
}

/*
    Counts total number of pages needed for the file
*/
int countTotalPages(FILE *f) {
    int totalLines = 0;
    char line[1000];
    while (fgets(line,999,f)) {
        totalLines++;
    }
    
    if (totalLines%4 != 0) {
        return ((totalLines/4) + 1);
    } else{
        return (totalLines/4);
    }
}


void loadPage(int pageNumber, FILE *f, int frameNumber) {
    rewind(f);  //sets the file position to the beginning of the file.
    int startPage       = pageNumber * 4;   // index for codes in the file
    int startFrame      = frameNumber * 4;  // index in RAM
    
    // Clear potential code from victim PCB
    ram[startFrame + 0] = NULL;
    ram[startFrame + 1] = NULL;
    ram[startFrame + 2] = NULL;
    ram[startFrame + 3] = NULL;

    int index           = 0;
    int numberOfLine    = 0;
    char line[1000];
    while (fgets(line,999,f)){
        if (index >= startPage){
            ram[startFrame] = strdup(line);
            numberOfLine++;
            startFrame++;
        }

        // Can load max 4 lines of code
        if (numberOfLine == 4){
            break;
        }
        index++;
    }
}

// This function finds a empty frame.
// it return -1 if there is no empty frame avaliable
int findFrame() {
    for (int i = 0; i < 10; i++){
        if (ram[i * 4] == NULL){
            return i;
        }
    }
    return -1;
}

/*
This function finds a victim frame by
Using a random number generator to pick a frame number
If the frame number does not belong to the pages of the active PCB and this frame is not being used 
by other PCBs, then eturn that frame number as the victim, 
Otherwise, iteratively increment the frame number (modulo-wise) until you come
to a suitable frame number, and return that number.
*/
int findVictim(PCB *p) {
    int random         = rand() % 10;
    
    while(1){
        int count      = 0;     // count the number of pagetable element not using this frame
        //check if the random(frame) is not in active PCB
        for(int i = 0; i < 10; i ++){
            if (random != p->pageTable[i]){
                count ++;
            }
        }

        struct ReadyQueueNode* temp = getHead();
        int FrameInUse = 0;   
        // Stop a PCB to steal another PCB's frame 
        // Check if this victim PCB is using this frame
        // i.e victim.PC = frame * 4
        while (temp != NULL){
            PCB* p = temp->PCB;
            if (p->PC == random * 4) {
                FrameInUse++;      
                break;
            }
            temp  = temp->next;
        }
        if (count == 10 && FrameInUse == 0) {
            return random;
        } else {
            random = (random + 1) % 10;
        }
    }

}

/*
The page table must be updated to reflect the changes. If a victim is invovled,
victim's table must be updated as well.
*/
int updatePageTable(PCB *p, int pageNumber, int frameNumber, int victimFrame) {
    if (frameNumber == -1){
        //update victim first
        struct ReadyQueueNode* temp = getHead();
        while (temp != NULL){
            PCB* p = temp->PCB;
            for (int i = 0; i < 10; i++){
                if (p->pageTable[i] == victimFrame) {
                    p->pageTable[i] = -1;
                break;
                }
            } 
            temp = temp->next;
        }

        //update active PCB
        p->pageTable[pageNumber] = victimFrame;
        return victimFrame;
    } else {
        p->pageTable[pageNumber] = frameNumber;
        return pageNumber;
    }
}
