#include "interpreter.h"
#include "shellmemory.h"
#include "shell.h"
#include "ram.h"
#include "pcb.h"
#include "cpu.h"


#include <string.h>
#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>


// lab3 stack example
typedef struct ReadyQueueNodeTag
{
    PCB pcb;
    struct ReadyQueueNodeTag *next;
} Node;

CPU* cpu;
Node *head = NULL;
Node *tail = NULL;

void addToReady(PCB *newpcb){
   
    if(!head && !tail){
        Node* newNode = (Node*) malloc(sizeof(Node));
        // memcpy will copy sizeof(PCB) to the newNode's field.
	    memcpy(&((*newNode).pcb), newpcb, sizeof(PCB));
	    (*newNode).next = NULL;
	    head = newNode;
        tail = newNode;
        //printf("head \n");
        
    }else{
        Node* newNode = (Node*) malloc(sizeof(Node));
        // memcpy will copy sizeof(PCB) to the newNode's field.
	    memcpy(&((*newNode).pcb), newpcb, sizeof(PCB));
        tail->next = newNode;
        tail = newNode;
        //printf("tail \n");
    } 
  
}


int myinit(char *filename){
   
   int start;
   int end;
   FILE *p = fopen(filename,"r");

   addToRAM(p, &start, &end);
   //printf("%d: \n", start);
  
   if(start == -1)
   {
       clearRAM();
       return 1;
   }
   
   addToReady(makePCB(start, end));
    return 0;
}

Node *pop(){
    Node *top = head;
    if (tail==head)
    {
        tail = NULL;
        head = NULL;
    }
    else
    {
        head = head->next;
    }
    return top;
}

int scheduler(){
    //printf("scheduler \n");
    Node *cur = pop();
    
    while(cur){
        //printf("1 \n");
        int x = (&((*cur).pcb))->PC;
        //printf("%d (x) \n",x);
        cpu->IP = x;    //  (&((*cur).pcb)).PC;
        //printf("2 \n");
        int line = (&((*cur).pcb))->end - (&((*cur).pcb))->PC;
        //printf("3 \n");
        if(line > 19){
            line = 20;
        }else{
            line++;
        }
        cpu->quanta = line;
        //printf("runbefore \n");
        runCPU(line);
        //printf("runCPU \n");

        (&((*cur).pcb))->PC = cpu->IP;
        //printf("%d (IP)\n",cpu->IP);
        //printf("%d (IP)\n",(&((*cur).pcb))->end);

        if(cpu->IP > (&((*cur).pcb))->end){
            deletePCB(&((*cur).pcb));
            //printf("delete \n");
        }else{
             addToReady(&((*cur).pcb));
        }
        
        cur = pop();
        //printf("pop \n");

    }
    //printf("out \n");
    clearRAM();
    return 0;
}

int main(void){
    printf("Kernel 1.0 loaded!\n");
    cpu = CPU_initialize();
    shellUI();
    return 0;
}


