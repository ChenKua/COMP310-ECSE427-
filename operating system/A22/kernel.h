#include "cpu.h"
#ifndef KERNEL_H
#define KERNEL_H

int myinit(char *filename);
int scheduler();
CPU* cpu;

#endif