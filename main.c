#include "stdio.h"
#include "stdlib.h"
#include "string.h"
#include "unistd.h"
#include <sys/wait.h>

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

#define SH_TOK_BUFSIZE 64
#define SH_TOK_DELIM " \t\r\n\a"
char **sh_split_line(char *line)
{
    int buf_size = SH_TOK_BUFSIZE;
    int position = 0;
    char **tokens = malloc(sizeof(char*) * buf_size);
    char *token;

    if(!tokens)
    {
        printf("sh: Memory Allocation Error!\n");
        exit(EXIT_FAILURE);
    }

    token = strtok(line, SH_TOK_DELIM);
    while(token!=NULL)
    {
        tokens[position] = token;
        position++;

        if(position>=buf_size)
        {
            buf_size += SH_TOK_BUFSIZE;
            tokens = realloc(tokens, buf_size);
            if(!tokens)
            {
                printf("sh: Memory Allocation Error!\n");
                exit(EXIT_FAILURE);
            }
        }
        token = strtok(NULL, SH_TOK_DELIM);
    }

    tokens[position] = NULL;
    return tokens;
}

int sh_launch(char **args)
{
    pid_t pid, wpid;
    int status;

    pid = fork();
    if(pid==0) //In Child Process
    {
        if(execvp(args[0], args) == -1){
            perror("sh");
        }
    }else if(pid<0){ //Forking Error
        perror("sh");
    }else{    //Parent Process
        do{
            wpid = waitpid(pid, &status, WUNTRACED);
        }while(!WIFEXITED(status) && !WIFSIGNALED(status));
    }
    return 1;
}

//Declaration of builtin functions
int sh_cd(char **args);
int sh_help(char **args);
int sh_exit(char **args);

char *builtin_str[] = {
    "cd",
    "help",
    "exit"
};

int (*builtin_func[]) (char **) = {
    &sh_cd,
    &sh_help,
    &sh_exit
};

int num_builtins(){
    return sizeof(builtin_str) / sizeof(char *);
}

int sh_cd(char **args)
{
    if(args[1] == NULL)
    {
        fprintf(stderr, "sh: Expected Argument to \"cd\"\n");
    }else{
        if(chdir(args[1])!=0)
        {
            perror("sh");
        }
    }
    return 1;
}

int sh_help(char **args)
{
    int i;
    printf("Simple Shell\n");
    printf("Type Program Names and Arguments, and hit enter. \n");
    printf("The following are builtin functions:\n");

    for(i = 0; i < num_builtins(); i++)
    {
        printf(" %s\n", builtin_str[i]);
    }

    printf("Use man for information on other programs.\n");
    return 1;
}

int sh_exit(char **args)
{
    exit(EXIT_SUCCESS);
    return 0;
}

int sh_execute(char **args)
{
    int i;
    if(args[0] == NULL){
        return 1;
    }

    //Find if it is in the builtin function list
    for(i=0; i< num_builtins(); i++)
    {
        if(strcmp(args[0], builtin_str[i])==0)
        {
            return (*builtin_func[i])(args);
        }
    }

    //If not, launch the program.
    return sh_launch(args);
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
        status = sh_execute(args);

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
