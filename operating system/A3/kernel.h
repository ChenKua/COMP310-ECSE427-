#include <stdio.h>
#include "pcb.h"

#ifndef KERNEL_H
#define KERNEL_H
/*
This is a node in the Ready Queue implemented as 
a linked list.
A node holds a PCB pointer and a pointer to the next node.
PCB: PCB
next: next node
*/
typedef struct ReadyQueueNode 
{
    PCB*  PCB;
    struct ReadyQueueNode* next;
} ReadyQueueNode;

/*
Adds a pcb to the tail of the linked list
*/
void addToReady(struct PCB*);

/*
Returns the size of the queue
*/
int size();

/*
Pops the pcb at the head of the linked list.
pop will cause an error if linkedlist is empty.
Always check size of queue using size()
*/
struct PCB* pop();

/*
Passes a filename
Opens the file, copies the content in the RAM.
Creates a PCB for that program.
Adds the PCB on the ready queue.
Return an errorCode:
ERRORCODE 0 : NO ERROR
ERRORCODE -3 : SCRIPT NOT FOUND
ERRORCODE -5 : NOT ENOUGH RAM (EXEC)
*/
int myinit(FILE *file, char *name);

int scheduler();

/*
Flushes every pcb off the ready queue in the case of a load error
*/
void emptyReadyQueue();

/*
Get the head of Queue
*/
struct ReadyQueueNode* getHead();

#endif