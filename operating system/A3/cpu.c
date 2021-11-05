#include<string.h>
#include"ram.h"
#include"interpreter.h"
#include"shell.h"
#include"memorymanager.h"

#define DEFAULT_QUANTA 2

struct CPU { 
    int IP; 
    char IR[1000]; 
    int quanta; 
    int offset;
} CPU;

/*
    A pageFault occurs when cpu offset offset reaches 4.
    It has two situation: 
        next page has valid frame
        next page has no valid frame
*/
int pageFault(PCB *p){
     printf("PageFault Occurs \n");
    p->PC_page     = p->PC_page + 1;
    int nextPage   = p->PC_page;

    //isOver
    if (nextPage > p->pages_max - 1) {
        return 0;
    }

    //Frame is valid then we have the frame number and can do
    //PC=ram[frame] and reset PC_offset to zero.
    if (p->pageTable[nextPage] != -1) {
        p->PC        = p->pageTable[nextPage] * 4;
        p->PC_offset = 0;
    } else {    
        // PageTable[PC_page] is NOT valid
        FILE *file = p->f;
        int frame = findFrame();
        if (frame == -1) {
            int victim = findVictim(p);
            updatePageTable(p, nextPage, -1, victim);
            loadPage(nextPage, file, victim);
        } else {
            updatePageTable(p, nextPage, frame, 0);
            loadPage(nextPage, file, frame);
        }
        p->PC_offset = 0;
        p->PC        = p->pageTable[nextPage] * 4;
    } 
    
    return 1;
}

/*
This method passes a quanta.
It will then executes quanta number of lines in RAM as from the address
pointed by the CPU IP.
Returns an errorCode.
*/
int run(int quanta){
    // If a quanta greater than the default quanta of 2 is passed,
    // run will instead execute only default quanta of lines.
    if (quanta > DEFAULT_QUANTA ){
        quanta = DEFAULT_QUANTA;
    }

    for (int i = 0; i < quanta; i++)
    {   
        strcpy(CPU.IR,ram[CPU.IP + CPU.offset]);
        int errorCode = parse(CPU.IR);
        // Do error checking and if error, return error
        if (errorCode != 0){
            // Display error message if fatal
            if (errorCode < 0){
                ram[CPU.IP][strlen(ram[CPU.IP])-2]='\0';
                displayCode(errorCode,ram[CPU.IP]);
            }
            return errorCode;
        }
        CPU.offset ++;

        //pseudo-interrupt: pagefault
        if(CPU.offset >= 4){
            return -99;
        }
    }
    return 0;
}