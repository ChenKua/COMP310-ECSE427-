#include <stdio.h>
#include <stdlib.h>
#include <string.h>


void swap(char *a){
    printf("%s\n",a);
}

int main(){

//    int *a=100;
//    int *b=100;
    char *ram[100];
    char *cmd = (char *)malloc(1000 * sizeof(char));
    ram[0] = "sadasdsada";
    char word[1000];
    char *str;

    str = ram[0];
    strcpy(cmd, ram[0]);
    printf("%s\n",cmd);

    free(cmd);
    cmd = NULL;

     free(cmd);
    cmd = NULL;

    char a[1000];
//    *b = 200;
   // printf("%d\n",a);
//    strcpy(a, ram[0]);
//    swap(a);
   //printf("%s\n",a);
//    printf("%s\n",str);
//    printf("%s\n",word);
     return 0;
}