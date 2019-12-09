#include "stdio.h"
#include "stdlib.h"
#include "string.h"

#define SH_BUF_SIZE 1024
char *sh_read_line()
{
    int buf_size = SH_BUF_SIZE;
    int position = 0;
    char *buffer = malloc(sizeof(char) * buf_size);
    int c;    //Using int instead of Char since EOF is a integer and we need to compare them

    if(!buffer)
    {
        printf("sh: Memory Allocation Error!\n");
        exit(EXIT_FAILURE);
    }

    while(1)
    {
        c = getchar();
        if(c == EOF || c == '\n')
        {
            buffer[position] = '\0';
            return buffer;
        }else{
            buffer[position] = c;
        }
        position++;

        if(position>=buf_size){
            buf_size += SH_BUF_SIZE;
            buffer = realloc(buffer, buf_size);

            if(!buffer){
                printf("sh: Memory Allocation Error!\n");
                exit(EXIT_FAILURE);
            }
        }
    }
}
//The main loop has three steps:
//1. Read, get the command and its args from stdin
//2. Parse, Split the input line into command and its args.
//3. Execute, run the command with its args
void sh_loop()
{
    char *line;
    char **args;
    int status;

    do{
        printf("> ");
        line = sh_read_line();
        args = sh_split_line(line);
        status = sh_exexute(args);

        free(line);
        free(args);
    }while(status);
}

//This main function reveals the general architecture of the program
//1. Initialization, loading config files
//2. Interpret the command read from stdin and executes them
//3. When the Terminate command received, free memories and shutdown the program
int main(int argc, char **argv)
{
    //Loading Config

    //Main Loop
    sh_loop();

    //Terminate program
    return EXIT_SUCCESS;
}
