#include <stdio.h>
#include <stdlib.h>
#include <string.h>


typedef struct{
    int IP; 
    char IR[1000]; 
    int quanta;
} CPU;

CPU* cpu;

CPU* initialize(){
    CPU *mycpu = (CPU *)malloc(sizeof(CPU));

    mycpu->IP = 0;
    mycpu->quanta = 0;

    return mycpu;
}

int main(){
    cpu = initialize();
    int x  =100;
    printf("%d\n", cpu->IP);
    cpu->IP = x;
    deletePCB();
    printf("%d\n", cpu->IP);
    return 0;

}

