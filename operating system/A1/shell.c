#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <time.h>
#include "interpreter.h"
clock_t start, end;
long cpu_time_used;

//global method
//int interpreter(char**, int);

//becase the last line of the txt file does not contain '\n' so we need to printf '\n' to adjust.
int haveTap(char ui[]){
    int a;
    for(a=0; ui[a]!='\0' && a<1000; a++){
        if(ui[a]=='\n'){
            return 1;
        }
    }
    return 0;
}

int parse(char ui[]){
    char tmp[200];
    char *words[100];
    int a,b;
    int w=0;

    // for(int i = 0;i< 100; i++){
	// 	words[i] = (char*) calloc(1, sizeof(char*));
	// }

    //printf("parse function==============\n");
    //  printf("%s\n", ui);

    //skip white space
   for(a=0; ui[a] == ' ' && a<1000; a++);
    

    while(ui[a] != '\0' && a<1000){
        for(b=0; ui[a]!='\0' && ui[a]!=' ' && ui[a]!='\n' && ui[a]!= '\t'&& a<1000; a++, b++){
            tmp[b] = ui[a]; 
        }
        tmp[b] = '\0';
        words[w] = strdup(tmp); 
        w++;

        if(ui[a]=='\n'){
            break;
        }

        for(a; (ui[a] == ' ' || ui[a]=='\n' || ui[a]=='\t') && a<1000; a++);
    }

    // printf("%d\n", w);
    // //testing purpose
    // int c=0;
    // while(c<w){
    //    printf("%s\n", words[c]);
    //    c++; 
    // }

    return interpreter(words,w);
}

int main() {
    
    //command
    char prompt[100] = {'$','\0'};
    char userInput[1000];
    int errorCode = 0;
    
    //

    //
    //start =clock(); 
    printf("Welcome to the <Kua Chen> shell!\n");
    //end = clock();
    //cpu_time_used = ((long) (end - start));
    // printf("%s %ld\n", "CPU cycles elapsed:", cpu_time_used);
    printf("Version 1.0 Created January 2020\n");   
 
   
    //  Take in user input
    while(1){
        printf("%s ",prompt);
    //scanf(" %[^\n]%*c", userInput);
        
        //$ get input from terminal or test file
        if(fgets(userInput, 999, stdin)==NULL){ 
            break;    
        }

        // if the input is not from the terminal ()
        if (!(isatty(STDIN_FILENO))){
            printf("%s",userInput);
                if(haveTap(userInput)==0){
                    printf("\n"); 
                }
        }
        

    //printf("%s\n", userInput);

        errorCode = parse(userInput);

        if(errorCode == -1){
            printf("Bye!\n");
            exit(99);
        }
        userInput[0]='\0';
    }
    return 0;

}