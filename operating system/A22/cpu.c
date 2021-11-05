#include <stdlib.h>
#include <stdlib.h>
#include <string.h>
#include <stdio.h>

#include "interpreter.h"
#include "cpu.h"
#include "ram.h"
#include "kernel.h"

CPU *CPU_initialize(){
    //printf("cpu init\n");
    CPU *cpu = (CPU*)malloc(sizeof(CPU));
   
    cpu->IP = 0;
    cpu->quanta = 0;
    return cpu;
}

// interpreter.c also has a run() method. Therefore, this method to runCPU
int runCPU(int quanta){
    
    for(int i=0; i<quanta; i++){
        strcpy(cpu->IR,ram[cpu->IP]);
        int result = interpret(cpu->IR);
        cpu->IP++;
    }

    return 0;
    
}