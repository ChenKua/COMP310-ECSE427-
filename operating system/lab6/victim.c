#include<stdlib.h>
#include<stdio.h>
#include<time.h>

int ram[100];

int maxPCB = 0;

struct PCB {
	int pid;
	int startAddress;
	int length;
} pcbList[20];

void addProcess(int pid, int size) {
	int j, count;

	maxPCB++;

	pcbList[pid].pid    = pid;
	pcbList[pid].length = size;

	// find a free space start
	for(j=0;j<100&&ram[j]!=-1;j++);
	pcbList[pid].startAddress = j;

	// check to see if there is enough space
	for(count=size;count>0 && ram[j]==-1 && j<100; count--, j++)
		ram[j] = pid;
	if (count!=0 || j>=100) pcbList[pid].startAddress = -1; // error
}

int victim(int pid, int size) {
	int j,k;
	int found = 0; // false

	for(j=0;j<maxPCB;j++) {
		if (pcbList[j].length >= size) {
			found = 1;
			pcbList[j].pid    = pid;
			pcbList[j].length = size;
			for(k=pcbList[j].startAddress;k<pcbList[j].startAddress+size;k++) ram[k]=pid;
			break;
		}
	}

	if(!found) addProcess(pid,size);

	return found;
}

void printRamPcb() {
	int i;

	// check ram and pcb list
	
	printf("Memory:\n");
	for(i=0;i<100;i++) printf("%d",ram[i]);
	printf("\n");
	printf("PCB list:\n");
	for(i=0;i<maxPCB;i++) printf("%d:a=%d,l=%d\n",pcbList[i].pid,pcbList[i].startAddress,pcbList[i].length);
}


int main() {
	int i, length;
	time_t t;

	for(i=0;i<100;i++) ram[i]=-1;

	// populate ram with 5 processes
	srand((unsigned) time(&t));

	for(i=0; i<5; i++) {
		length = rand() % 20;
		addProcess(i,length);
	}

	// check ram and pcb list
	printRamPcb();

	// generate a random new process
	i=6;
	length = rand() % 20;
	printf("***New Process: pid: %d, length: %d\n", i, length);

	// select a victim and overwrite (if space)
	if (victim(i,length)) printf("Victim found\n");
	else printf("Victim not found. Added to end.");

	// check ram and pcb list
	printRamPcb();
}
