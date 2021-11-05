#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <time.h>
#include "shell.h"
#include "shellmemory.h"

clock_t start, end;
long cpu_time_used;


//#include shellmemory.h
//int input(char *name, char *val);
//char *getVal(char *name);

//#include shell.h
//int parse(char ui[]);


//testing purpose
void test(char ui[]){
    int a;
    for(a=0; ui[a]!='\0' && a<1000; a++){
        printf("%c",ui[a]);
    }
    printf(" \n");
}

int set(char *words[]){
    return input(words[1],words[2]);
}

int print(char *words[]) {
	// printf(getVal(words[1]),"%s");
	// printf("\n");
	// return 0;
    char *var = strdup(words[1]);
    char* val= getVal(var);
        printf("%s", val);
        printf("\n");
	    return 0;
}

int recursiveCount=0;

int run(char *words[]){
    int err = 0;
    char line[1000];
    // char example[] = "testing testing";
    // char * fileName = strdup(words[1]);
    // opens a file for reading
    //printf("%s\n",words[1]);

    //start =clock(); 
    
    
    FILE *p = fopen(words[1],"r"); 
//end = clock();
    //cpu_time_used = ((long) (end - start));
    // printf("%s %ld\n", "CPU cycles elapsed:", cpu_time_used);
    if (!p){
		printf("Script not found \n");
        recursiveCount = 0;
		return err;
	}

    if ( recursiveCount > 128) {
        printf("TERMINATED:This script has a an infinite loop!\n");
        fclose(p);
        return 0;
    }
    recursiveCount++;
    
    //fgets(line,999,file);
    while (fgets(line,999,p))
    {   
        //test(line);
        printf("$ %s",line);
        //start = clock();
        
    
        if(feof(p)){
            //end = clock();
    //cpu_time_used = ((long) (end - start));
    // printf("%s %ld\n", "CPU cycles elapsed:", cpu_time_used);
            printf("\n");
        }
        err = parse(line);// read each line of the file
        //printf("script %d\n",err);
        if(err == 1){
            fclose(p);
            printf("Command syntax error \n");
            recursiveCount = 0;
            return err;
        }else if(err == -1){
            recursiveCount = 0;
            return err;
        }
    }

	fclose(p); // close file
    recursiveCount = 0;
	return err;



    // fgets(line,999,file);
    // while(!feof(file)){
    //     err = parse(line);
    //     if(err != 0){
    //         fclose(file);
    //         printf("Command syntax error \n");
    //         return err;
    //     }
    //     fgets(line,999,file);
    // }
    // fclose(file);
    // return err;
}

int interpreter(char* words[], int w){
    //printf("interpreter %d.\n",w);
    //printf("interpreter %s.\n",words[0]);

    int errorCode =0;
    // 0=help,set,print,run
    // 1=unknown command,
    //-1=quit
    
    //printf("%s\n",words[0]);

    //= 0           String pointed to by string1 equivalent to string pointed to by string2
    
   //start = clock();
    
    if(strcmp(words[0],"help")==0 && w==1){   
        //end = clock();
    //cpu_time_used = ((long) (end - start));
    // printf("%s %ld\n", "CPU cycles elapsed:", cpu_time_used);  
        printf("COMMAND                     DESCRIPTION\n");
        printf("                   \n");
        printf("help                        Displays all the commands\n");
        printf("quit                        Exits / terminates the shell with “Bye!”\n");
        printf("set VAR STRING              Assigns a value to shell memory\n");
        printf("run SCRIPT.TXT              Executes the file SCRIPT.TXT\n");
        errorCode = 0;
    }else if((strcmp(words[0],"quit")==0 && w==1)){
        errorCode = -1;

    }else if((strcmp(words[0],"set")==0 && w==3)){
        errorCode = set(words);

    }else if ((strcmp(words[0], "print")==0 && w==2)) {
		errorCode = print(words);
	}
	else if (strcmp(words[0], "run")==0 && w==2) {
		//printf("Run\n");
        errorCode = run(words);
	}
    else {
        printf("Unknown command \n");
        errorCode = 1;
    }
    return errorCode;
}