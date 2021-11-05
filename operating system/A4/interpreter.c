#include<stdio.h>
#include<string.h>
#include<stdlib.h>
#include"shellmemory.h"
#include"shell.h"
#include"kernel.h"
#include"ram.h"
#include"memorymanager.h"
#include"DISK_driver.h"

#define TRUE 1
#define FALSE 0

/*
This function takes an array of string. 
First string is the "set" command
Second string is a variable name
Third string is a value
It assigns that value to a environment variable with that variable name in the shell memory array.
Return ERRORCODE -1 if out of memory else 0
*/
int set(char * words[]){
    char* varName = words[1];
    char* value = words[2];
    int errorCode = setVariable(varName,value);
    return errorCode;
}

/*
This function takes an array of string. 
First string is the "print command".
Second string is the variable name.
It will print the value associated with that variable if it exists.
Else it will print an appropriate message.
Return 0 if successful.
*/
int print(char * words[]){

    char* varName = words[1];
    char* value = getValue(varName);

    if (strcmp(value,"_NONE_")==0) {
        // If no variable with such name exists, display this message
        printf ("Variable does not exist\n");
    } else {
        // else display the variable's value
        printf("%s\n",value);
    }
    return 0;
}

/*
This function takes an array of string.
First string is the "run" command
Second string is script filename to execute
Returns errorCode
*/
static int run(char * words[]){

    char * filename = words[1];
    FILE * fp = fopen(filename,"r");
    int errorCode = 0;
    // if file cannot be opened, return ERRORCODE -3
    if (fp==NULL) return -3;
    char buffer[1000];
    printf("/////////////////////////////// STARTING EXECUTION OF %s ///////////////////////////////\n",filename);
    while (!feof(fp)){
        fgets(buffer,999,fp);
        errorCode = parse(buffer);
        // User input the "quit" command. Terminate execution of this script file.
        if (errorCode == 1) {
            // Run command successfully executed so ERRORCODE 0. Stop reading file.
            errorCode = 0;
            break;
        } else if (errorCode != 0) {
            // An error occurred. Display it and stop reading the file.
            //removing the extra carriage return
            buffer[strlen(buffer)-2]='\0';
            displayCode(errorCode,buffer);
            break;
        }
    }
    printf("/////////////////////////////// Terminating execution of %s ///////////////////////////////\n",filename);
    fclose(fp);
    return 0;
}

int exec(char * words[]){
    char * filename[3] = { "_NONE_", "_NONE_", "_NONE_"};
    int errorCode = 0;
    for (int i = 0; i < 3; i++)
    {
        if ( strcmp(words[i+1],"_NONE_") != 0 ) {
            filename[i] = strdup(words[i+1]);
            FILE * fp = fopen(filename[i],"r");
            if (fp == NULL) {
                errorCode = -3;
            } else {
                int err = launcher(fp);
                // if launcher failed, set errorCode to -6 for LAUNCHING ERROR
                if ( err == 0){
                    errorCode = -6;
                }
            }
            if ( errorCode < 0){
                displayCode(errorCode,words[i]);
                printf("EXEC COMMAND ABORTED...\n");
                emptyReadyQueue();
                clearRAM();
                return 0;
            }
        }
    }

    printf("|----------| ");
    printf("\tSTARTING CONCURRENT PROGRAMS ( ");
    for (int i = 0; i < 3; i++)
    {
        if ( strcmp(filename[i],"_NONE_") != 0 ){
            printf("%s ", filename[i]);
        }
    }
    printf(")\t|----------|\n");

    scheduler();

    printf("|----------| ");
    printf("\tTERMINATING ALL CONCURRENT PROGRAMS");
    printf("\t|----------|\n");
    return 0;
}

// <---Added method for A4--->

/*
Mount the partition.
If a partition already exists with the name provided then the existing partition is mounted. 
Otherwise the provided arguments are used to create and mount the new partition.
*/
int mount(char * words[]){
    printf("---------------------------------------------\n");
    printf("-------------- Mount File: %s ---------------\n", words[1]);
    printf("---------------------------------------------\n");
    int blocksize   = atoi(words[3]); //translate string to int
    int totalblocks = atoi(words[2]); //translate string to int
    int err = 0;

    if (blocksize == 0 || totalblocks == 0) {
        printf("<ERROR>: incorrect block_size or incorrect total_blocks");
        return -7;
    }
    
    // Find the route to the partition file
    char route[1000];
    strcpy(route, "PARTITION/");
    strcat(route, words[1]);
    FILE *partitionFile = fopen(route, "r");

    int err1            = -1;
    int err2            = -1;

    // If a partition already exists
    if(partitionFile){
        fclose(partitionFile);
        err2 = mountFS(words[1]);           
    }else{  // do not exist. Create a new one
        err1 = partition(words[1], blocksize, totalblocks);
        err2 = mountFS(words[1]);
    }
    
    if (err1 == 0){
        return -7;
    }
    if (err2 == 0){
        return -7;
    }
    
    return 0;
}

/*
Write a collection of words into file
Return -8 for any error
*/
int write(char * words[]){
    printf("---------------------------------------------\n");
    printf("-------------- Write File: %s ---------------\n", words[1]);
    printf("---------------------------------------------\n");
    char data[1000];
    
    //Clear the array data[1000]
    for (int i = 0; i < 1000; i++) {
        data[i] = '\0';
    }
    
    // Copy words[] into data[]
    for (int i = 2; i < 100; i++) {
        if (strcmp(words[i],"_NONE_") == 0) {
            break;
        }            
        strcat(data, words[i]);
    }

    // Count the total number of charaters
    int count = strlen(data);
    char data2[1000];  //data without remove bracket
    int index = 0;
    // remove brackets
    printf("Compact data...Remove brakets...Remove blank space\n");
    for (int i = 1; i < count - 1; i++) {
       data2[index] = data[i];
       index++;
    }
    data2[index] = '\0';  

    printf("Data: %s \n", data2);  
    
    int file = openfile(words[1]);
    int err  = writeBlock(file, data2);
    if (err == -1) {
        return -8;
    }
    return err;
}

/*
Read data in the block of BlockPointer[current_location].
Read 1 block at 1 time.
Return -9 for any error.
*/
int read(char * words[]){
    int file = openfile(words[1]);

    // file is not in the file table
    if (file == -1) {
        return -9;
    }

    char *value   = readBlock(file);
    int errorCode = 0;
   
    // If value is NULL or value "000000" i.e. block is empty
    printf("-----------------------------------------\n");
    printf("----------- Read File: %s ---------------\n", words[1]);
    printf("-----------------------------------------\n");
    if (value == NULL || value[0] == '0') {
        char *empty = "NULL";
        printf("<ERROR>: Read NULL\n");
        //errorCode   = setVariable(words[2], empty);
        return -9;
    } 
    
    // read the entire file not just one block
    char entireFile[1000];
    strcpy(entireFile, value);

    while(value != NULL){
        value = readBlock(file);
        if (value == NULL || value[0] == '0')
            break;
        strcat(entireFile, value);
    }     
    // Write the string to the shell variable
    setVariable(words[2], entireFile);
    return 0;

}

// <---Above method--->

/*
This functions takes a parsed version of the user input.
It will interpret the valid commands or return a bad error code if the command failed for some reason
Returns:
ERRORCODE  0 : No error and user wishes to continue
ERRORCODE  1 : Users wishes to quit the shell / terminate script
ERRORCODE -1 : RAN OUT OF SHELL MEMORY
ERRORCODE -2 : INCORRECT NUMBER OF ARGUMENTS
ERRORCODE -3 : FILE DOES NOT EXIST
ERRORCODE -4 : UNKNOWN COMMAND. TRY "help"
*/
int interpreter(char* words[]){
    //default errorCode if no error occurred AND user did not enter the "quit" command
    int errorCode = 0;
    //At this point, we are checking for each possible commands entered
    if ( strcmp(words[0],"help") == 0 ) {
        
        // if it's the "help" command, we display the description of every commands
        printf("-------------------------------------------------------------------------------------------------------\n");
        printf("COMMANDS\t\t\tDESCRIPTIONS\n");
        printf("-------------------------------------------------------------------------------------------------------\n");
        printf("help\t\t\t\tDisplays all commands\n");
        printf("quit\t\t\t\tTerminates the shell\n");
        printf("set VAR STRING\t\t\tAssigns the value STRING to the shell memory variable VAR\n");
        printf("print VAR\t\t\tDisplays the STRING value assigned to the shell memory variable VAR\n");
        printf("run SCRIPT.TXT\t\t\tExecutes the file SCRIPT.txt\n");
        printf("exec p1 p2 p3\t\t\tExecutes programs p1 p2 p3 concurrently\n");
        printf("mount partitionName number_of_blocks block_size          Create and mount the new partition\n");
        printf("write filename [a collection of words]                   Write into file\n");
        printf("read filename variable                                   Read from file\n");
        printf("-------------------------------------------------------------------------------------------------------\n");

    } else if ( strcmp(words[0],"quit") == 0) {

        // if it's the "quit" command
        //errorCode is 1 when user voluntarily wants to quit the program.
        errorCode = 1;

    } else if ( strcmp(words[0],"set") == 0 ) {
        // if it's the "set VAR STRING" command
        // check for the presence or 2 more arguments
        // If one argument missing, return ERRORCODE -2 for invalid number of arguments
        if ( ( strcmp(words[1],"_NONE_") == 0 ) || ( strcmp(words[2],"_NONE_") == 0 ) ) {
            errorCode = -2;
        } else {
            // ERRORCODE -1 : Out of Memory might occur
            errorCode = set(words);
        }
    }  else if ( strcmp(words[0],"print") == 0 ) {
        // if it's the "print VAR" command
        // if there's no second argument, return ERRORCODE -2 for invalid number of arguments
        if ( strcmp(words[1],"_NONE_") == 0 ) return -2;

        // Call the print function
        errorCode = print(words);

    } else if ( strcmp(words[0],"run") == 0 ) {
        // if it's the "run SCRIPT.TXT" command
        // check if there's a second argument, return ERRORCODE -2 for invalid number of arguments
        if ( strcmp(words[1],"_NONE_") == 0 ) return -2;

        //Error will be handled in the run function. We can assume that after the run 
        //function terminate, the errorCode is 0.
        errorCode = run(words);
    } else if ( strcmp(words[0],"exec") == 0 ) {
        // if it's the "exec" command
        // check if there's at least 2 arguments and not >= 4 arguments
        if ( strcmp(words[1],"_NONE_") == 0  || strcmp(words[4],"_NONE_") != 0 ) return -2;

        errorCode = exec(words);
    } else if (strcmp(words[0],"mount") == 0) {
        //Pattern: mount partitionName number_of_blocks block_size
        if ( ( strcmp(words[3],"_NONE_") == 0 ) || ( strcmp(words[4],"_NONE_") != 0 ) ) {
            return -2;
        }
        errorCode = mount(words);

    } else if (strcmp(words[0],"read") == 0) {
        //Pattern: read filename variable
        if ( ( strcmp(words[2],"_NONE_") == 0 ) || ( strcmp(words[3],"_NONE_") != 0 ) ) {
            return -2;
        }
        errorCode = read(words);

    } else if (strcmp(words[0],"write") == 0) {
        if ( strcmp(words[2],"_NONE_") == 0 ) {
            return -2;
        } 
        printf("Write following words into file\n");
        printf("-------------------------------------------------------------------------------------------------------\n");
        for (int i = 2; i < 100; i++) {
            if (strcmp(words[i],"_NONE_") == 0) {
                break;
            }
            printf("%s \n", words[i]);
        }      
        printf("-------------------------------------------------------------------------------------------------------\n");      
        errorCode = write(words);
        
    } else {
        // Error code for unknown command
        errorCode = -4;
    }

    return errorCode;
    
}
