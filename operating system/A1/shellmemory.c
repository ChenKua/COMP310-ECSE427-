#include<stdlib.h>
#include<stdio.h>
#include <string.h>

// class in java
struct MEM {
    char *name;
    char *val;
};


//private array
struct MEM Memory[1000];  

    // 0=help,set,print,run
    // 1=unknown command,
    //-1=quit

int counter;

int input(char* name, char* val){


    for (int a = 0; a < 1000; a++)
    {
        
        if (Memory[a].name == NULL){    // input new var and new value
            Memory[a].name = strdup(name);
            //  printf("%s\n", name);
            Memory[a].val = strdup(val);
            //  printf("%s\n", val);
            counter ++;
            //printf("%d\n", counter);
            return 0;
        }

        // if name exists
        if (strcmp(Memory[a].name, name)==0){   // for updating value
            //  printf("%s\n", val);
            Memory[a].val = val;
            counter ++;
            //printf("%d\n", counter);   
            return 0;
        }

        
    }
    

    if (counter == 1000){    // array is full
        printf("Shell memory full\n");
    }

    return 1;
}

// get value from name
char *getVal(char *name) {
	 for (int b = 0; b < 1000; b++)
    {
        if (Memory[b].name == NULL){
            //  printf("%s\n", name);
            return "Variable does not exist";
        }
        if (strcmp(Memory[b].name, name)==0){
            return Memory[b].val;
        }    
    }
    // may no need this line
	return "Variable does not exist";
}



