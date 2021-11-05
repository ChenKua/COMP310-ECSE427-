#include<stdio.h>
#include<stdlib.h>
#include <sys/stat.h>
#include <sys/types.h>
#include"shell.h"
#include"pcb.h"
#include"ram.h"
#include"cpu.h"
#include"interpreter.h"
#include"memorymanager.h"
#include"kernel.h"


ReadyQueueNode* head = NULL;
ReadyQueueNode* tail = NULL;
int sizeOfQueue = 0;

/*
Get the head of the ReadyQueue
*/
struct ReadyQueueNode* getHead() {
    return head;
}

int kernel() {
    return shellUI();
}

void boot() {
    removeFromRam(0,39);    // clear RAM
    
    // Backing store
    // if success = 0
    // if already exist a folder = -1
    int result = mkdir("BackingStore", 0777);

    if(result == -1)
    {
        system("rm -rf BackingStore");
        mkdir("BackingStore", 0777);
    }
}

int main(int argc, char const *argv[]) {
    int error=0;
    boot();               // First : actions performed by boot
    error = kernel();     // Second: actions performed by kernel
    return error;   
}

/*
Adds a pcb to the tail of the linked list
*/
void addToReady(struct PCB* pcb) {
    ReadyQueueNode* newNode = (ReadyQueueNode *)malloc(sizeof(ReadyQueueNode));
    newNode->PCB = pcb;
    newNode->next = NULL;
    if (head == NULL){
        head = newNode;
        tail = newNode;
    } else {
        tail->next = newNode;
        tail = newNode;
    }
    sizeOfQueue++;
}

/*
Returns the size of the queue
*/
int size(){
    return sizeOfQueue;
}

/*
Pops the pcb at the head of the linked list.
pop will cause an error if linkedlist is empty.
Always check size of queue using size()
*/
struct PCB* pop(){
    PCB* topNode = head->PCB;
    ReadyQueueNode * temp = head;
    if (head == tail){
        head = NULL;
        tail = NULL;
    } else {
        head = head->next;
    }
    free(temp);
    sizeOfQueue--;
    return topNode;
}

/*
Passes a file
load the pages in the RAM.
Creates a PCB for that program.
Adds the PCB on the ready queue.
Return an errorCode:
ERRORCODE 0 : NO ERROR
ERRORCODE -3 : FILE NOT FOUND
*/
int myinit(FILE* file, char *filename){
    // call addToRam on that File *
    // If error (check via start/end variable), return that error
    // Else create pcb using MakePCB
    // Then add it to the ReadyQueue

    if (file == NULL) return -3;

    int totalPages = countTotalPages(file);
    printf("With %d total pages \n", totalPages);
    int frame;
    frame = findFrame();

    //If the program has 4 or less lines of code, then only one page is loaded. 
    //If the program has more than 8 lines of code, then only the first two pages are loaded 
    PCB* pcb = makePCB(totalPages, file, filename);
    if (totalPages <= 1) {
        if (frame == -1) {
            int victim = findVictim(pcb);
            updatePageTable(pcb, 0, -1, victim);       // update victim pagetable first
            loadPage(0,file,victim);                   // then load into RAM
        } else {
            loadPage(0,file,frame);
            updatePageTable(pcb, 0, frame, 0);
        }
    } else {
       for (int i = 0; i < 2; i++) {
            frame = findFrame();
            if (frame == -1) {
                frame = findVictim(pcb);
                updatePageTable(pcb, i, -1, frame);     //input order: pcb , pageNum, frame, victim
                loadPage(i,file,frame);   
            } else {
                updatePageTable(pcb, i, frame, 0); 
                loadPage(i,file,frame);   
            }
       } 
    } 
    pcb->PC = pcb->pageTable[0] * 4;
    addToReady(pcb);
    return 1;
}

// Remove valid frame of a PCB by its pagetable
void removeFromRamByTable(int table[]) {
    for (int i = 0; i < 10; i++) { // each 4 cells of the array is a frame
        if (table[i] != -1) {
            removeFromRam(table[i], table[i] + 3);
        }    
    }
}

int scheduler(){
    // set CPU quanta to default, IP to -1, IR = NULL
    CPU.quanta = DEFAULT_QUANTA;
    CPU.IP = -1;
    while (size() != 0){
        //pop head of queue
        PCB* pcb   = pop();
        //copy PC of PCB to IP of CPU
        CPU.IP     = pcb->PC;
        CPU.offset = pcb->PC_offset;
        int isOver = FALSE;
        int quanta = DEFAULT_QUANTA;
        int remainLineInFrame = 0;             // lines of code in the frame
        for (int i = CPU.IP; i < CPU.IP + 4; i++) {
            if (ram[i] != NULL) remainLineInFrame++;
        }
       
        // number of unexecuted lines in the frame
        if (DEFAULT_QUANTA >= remainLineInFrame - CPU.offset) {
            quanta = remainLineInFrame - CPU.offset;
        }

        int errorCode  = run(quanta);
        pcb->PC_offset = CPU.offset;
        
        // if pageFault happens
        if (errorCode == -99) {
            int result = pageFault(pcb);
            if (result == 0 ){  //isOver
                isOver = TRUE;
                printf("<- End Of PCB: %s ->\n", pcb->name);
            } else {
                isOver = FALSE;
            }
            errorCode  = 0;
        } else {
            // End of the frame but do not triger a pagefault
            // i.e only 1/2/3 lines of code in the frame
            // a frame that is not full implies this is the final frame
            if (pcb->PC_offset >= remainLineInFrame && pcb->PC_page >= pcb->pages_max -1) {
                isOver = TRUE;
                printf("<- End Of PCB: %s ->\n", pcb->name);
            } 
        }

        if ( errorCode!=0 || isOver ){
            removeFromRamByTable(pcb->pageTable);
            fclose(pcb->f);
            free(pcb);
        } else {
            addToReady(pcb);
        }
    }
    // reset RAM
    resetRAM();
    return 0;
}

/*
Flushes every pcb off the ready queue in the case of a load error
*/
void emptyReadyQueue(){
    while (head!=NULL){
        ReadyQueueNode * temp = head;
        head = head->next;
        free(temp->PCB);
        free(temp);
    }
    sizeOfQueue =0;
}

