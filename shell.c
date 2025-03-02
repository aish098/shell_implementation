#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/wait.h>

#define MAX_INPUT_SIZE 1024
#define MAX_ARGS 64


void print_prompt();
char* read_input();
char** parse_input(char* input);
int execute_builtin(char** args);
void execute_command(char** args);

int main() {
    char *input;
    char **args;
    int status = 1;

    while (status) {
        print_prompt();
        input = read_input();
        
        if (input == NULL) continue;
        
        args = parse_input(input);
        
        if (args[0] != NULL) {
            if (!execute_builtin(args)) {
                execute_command(args);
            }
        }

       
        free(input);
        free(args);
    }

    return 0;
}

void print_prompt() {
    printf("myshell> ");
    fflush(stdout);
}

char* read_input() {
    char *input = malloc(MAX_INPUT_SIZE);
    if (!input) {
        perror("malloc error");
        exit(1);
    }

    if (fgets(input, MAX_INPUT_SIZE, stdin) == NULL) {
        free(input);
        return NULL;
    }


    input[strcspn(input, "\n")] = 0;
    return input;
}

char** parse_input(char* input) {
    char **args = malloc(MAX_ARGS * sizeof(char*));
    if (!args) {
        perror("malloc error");
        exit(1);
    }

    char *token = strtok(input, " ");
    int position = 0;

    while (token != NULL && position < MAX_ARGS - 1) {
        args[position] = token;
        position++;
        token = strtok(NULL, " ");
    }
    args[position] = NULL;
    
    return args;
}

int execute_builtin(char** args) {
    if (strcmp(args[0], "exit") == 0) {
        exit(0);
    }
    
    if (strcmp(args[0], "cd") == 0) {
        if (args[1] == NULL) {
            fprintf(stderr, "cd: missing argument\n");
        } else {
            if (chdir(args[1]) != 0) {
                perror("cd error");
            }
        }
        return 1;
    }
    
    return 0;
}

void execute_command(char** args) {
    pid_t pid = fork();

    if (pid < 0) {
        perror("fork error");
        exit(1);
    }
    
    if (pid == 0) { 
        execvp(args[0], args);
        
        perror("exec error");
        exit(1);
    } else {  
        wait(NULL);
    }
}