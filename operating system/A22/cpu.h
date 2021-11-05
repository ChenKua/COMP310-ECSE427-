#ifndef CPU_H
#define CPU_H

typedef struct{
    int IP; 
    char IR[1000]; 
    int quanta;
} CPU;

CPU *CPU_initialize();

int runCPU(int quanta);

#endif