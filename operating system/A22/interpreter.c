#include "interpreter.h"
#include "shellmemory.h"
#include "kernel.h"

#include <time.h>
#include <stdlib.h>
#include <stdint.h>
#include <string.h>
#include <stdio.h>

char **tokenize(char *str)
{
    size_t num_tokens = 1;
    int flag = 0;
    for (size_t i = 0; i < strlen(str); i++)
    {
        if (flag == 0 && str[i] == ' ')
        {
            num_tokens = num_tokens + 1;
            flag = 1;
        }
        if (str[i] != ' ')
        {
            flag = 0;
        }
    }
    char **ret_arr =
        (char **)malloc(sizeof(char *) * (num_tokens + 1));

    if (ret_arr == NULL)
    {
        perror("malloc");
        return NULL;
    }
    flag = 0;
    int ignore_flag = 0;
    char *modified_str = (char *)str;
    size_t counter = 0;
    const size_t length_str = strlen(str);
    for (size_t i = 0; i < length_str; i++)
    {
        if (modified_str[i] == '\n' || modified_str[i] == '\r')
            modified_str[i] = ' ';
        if (modified_str[i] == '"')
        {
            ignore_flag = ignore_flag ^ 0x1;
        }
        if (flag == 0 && modified_str[i] != ' ')
        {
            ret_arr[counter] = &(modified_str[i]);
            counter = counter + 1;
            flag = 1;
        }
        if (modified_str[i] == '\\' && modified_str[i + 1] == ' ')
        {
            i++;
            continue;
        }
        if (flag == 1 && modified_str[i] == ' ' && ignore_flag == 0)
        {
            modified_str[i] = '\0';
            flag = 0;
            continue;
        }
    }
    ret_arr[counter] = NULL;

    for (size_t i = 0; i < counter; ++i)
    {
        if (ret_arr[i][0] == '\"' &&
            ret_arr[i][strlen(ret_arr[i] - 1)] == '\"')
        {
            ret_arr[i][strlen(ret_arr[i]) - 1] = '\0';
            ret_arr[i] = ret_arr[i] + 1;
        }
    }

    return ret_arr;
}

int in_file_flag = 0;
int interpret(char *raw_input);

int help()
{
    printf(""
           "COMMAND         DESCRIPTION\n"
           "help            Displays all the commands\n"
           "quit            Exits / terminates the shell with \"Bye!\"\n"
           "set VAR STRING  Assigns a value to shell memory\n"
           "print VAR       Displays the STRING assigned to VAR\n"
           "run SCRIPT.TXT  Executes the file SCRIPT.TXT\n");
    return 0;
}

int quit()
{
    printf("Bye!\n");
    if (in_file_flag == 0)
    {
        shell_memory_destory();
        exit(0);
    }
    return 0;
}

int run(const char *path)
{
    FILE *file = fopen(path, "r");
    if (file == NULL)
    {
        printf("Script not found.\n");
        return 1;
    }

    int err = 0;
    char line[1000];
    while (fgets(line,999,file))
    {   
        
        err = interpret(line); // read each line of the file
        //printf("script %d\n",err);
        if(err == 1){
            fclose(file);
            //printf("Command syntax error \n");
            
            return err;
        }

    }

	fclose(file); // close file
	return err;

    // int enter_flag_status = in_file_flag;
    // in_file_flag = 1;
    // while (!feof(file))
    // {   
    //     char *line = NULL;
    //     size_t linecap = 0;
    //     getline(&line, &linecap, file);

    //     int status = interpret(line);
    //     printf("inter.\n");
    //     free(line);
    //     printf("free.\n");
    //     printf("%d.\n",status);
    //     if (status != 0)
    //     {   
    //         printf("break\n");
    //         break;
    //         printf("return\n");
    //         return status;
    //     }
    // }
    // printf("out while.\n");
    // fclose(file);
    // in_file_flag = enter_flag_status;
    // printf("end.\n");
    // return 0;
}

int set(const char *key, const char *value)
{
    int status = shell_memory_set(key, value);
    if (status != 0)
        printf("set: Unable to set shell memory.\n");
    return status;
}

int print(const char *key)
{
    const char *value = shell_memory_get(key);
    if (value == NULL)
    {
        printf("print: Undefiend value.\n");
        return 1;
    }
    printf("%s\n", value);
    return 0;
}

// Starting the execution
int exec(char **tokens){
    clock_t start, end;
    long cpu_time_used;
    start = clock();

    int i = 0; // 1,2,3,4
    while(tokens[i]!=0){
        i++;
    }
    //only 2 inpur P. Check if p1 = p2
    if((i==3) && (strcmp(tokens[1], tokens[2]) == 0)){
        printf("Error: Script <%s> already loaded\n",tokens[1]);
        return 1;    
    }
    //in case of 3 program
    if(i==4){
        if(strcmp(tokens[1], tokens[2]) == 0)
        {
            printf("Error: Script <%s> already loaded\n",tokens[1]);
            return 1;
        }
        if(strcmp(tokens[2], tokens[3]) == 0)
        {
            printf("Error: Script <%s> already loaded\n",tokens[2]);
            return 1;
        } 
        if(strcmp(tokens[1], tokens[3]) == 0)
        {
            printf("Error: Script <%s> already loaded\n",tokens[1]);
            return 1;
        }   
    }

    // if the files does exist
    for(int a=1; a<i;a++){
        FILE *p = fopen(tokens[a],"r"); 
        if(!p){
            printf("Error: Script <%s> does not exist\n", tokens[a]);
            return 1;
        }
        fclose(p); // close file
    }

    // start myinit()
    for(int a = 1; a<i;a++){
        int result = myinit(tokens[a]);
        if(result == 1)
        {
            return 1;
        }
    }

    scheduler();
    //printf("schedulaer finished\n");

    end = clock();
    cpu_time_used = ((long) (end - start));
    //printf("%s %ld\n", "CPU cycles elapsed:", cpu_time_used);
    return 0;


}

///////////////
// interpret //
///////////////
int interpret(char *raw_input)
{   
    char **tokens = tokenize(raw_input);

    // Go to exec function in kernel
    if(strcmp(tokens[0], "exec") == 0)
    {
        // Wrong case: exec p1 p2 p3 p4
        if(tokens[4] != NULL)
        {
            printf("help: Malformed command\n");
            free(tokens);
            return 1;
        }

        // Wrong case: exec 
        if(tokens[1] == NULL)
        {
            printf("help: Malformed command\n");
            free(tokens);
            return 1;
        }
        
        return exec(tokens);
        
    }

    if (tokens[0] == NULL)
        return 0; // empty command

    if (strcmp(tokens[0], "help") == 0)
    {
        if (tokens[1] != NULL)
        {
            printf("help: Malformed command\n");
            free(tokens);
            return 1;
        }
        free(tokens);
        return help();
    }

    if (strcmp(tokens[0], "quit") == 0)
    {
        if (tokens[1] != NULL)
        {
            printf("quit: Malformed command\n");
            free(tokens);
            return 1;
        }
        free(raw_input);
        free(tokens);
        return quit();
    };

    if (strcmp(tokens[0], "set") == 0)
    {
        if (!(tokens[1] != NULL && tokens[2] != NULL && tokens[3] == NULL))
        {
            printf("set: Malformed command\n");
            free(tokens);
            return 1;
        }
        int status = set(tokens[1], tokens[2]);
        free(tokens);
        return status;
    }

    if (strcmp(tokens[0], "print") == 0)
    {
        if (!(tokens[1] != NULL && tokens[2] == NULL))
        {
            printf("print: Malformed command\n");
            free(tokens);
            return 1;
        }
        int status = print(tokens[1]);
        free(tokens);
        return status;
    }

    if (strcmp(tokens[0], "run") == 0)
    {
        if (!(tokens[1] != NULL && tokens[2] == NULL))
        {
            printf("run: Malformed command\n");
            free(tokens);
        }
        int result = run(tokens[1]);
        free(tokens);
        return result;
    }

    printf("Unrecognized command \"%s\"\n", tokens[0]);
    free(tokens);
    return 1;
}