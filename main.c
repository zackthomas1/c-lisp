#include <stdio.h>
#include <stdlib.h>

// Declare a bufffer for user input of size 2048
static char buffer[2048];

// If compiling on Windows include these libraries
#ifdef _WIN32
#include <string.h>

char* readline(char* prompt){ 
    fputs(prompt, stdout); 
    fgets(buffer, 2048, stdin);
    char* cpy = malloc(strlen(buffer)+1); 
    strcpy(cpy, buffer);
    cpy[strlen(cpy)-1] = '\0';
    return cpy;
}

void add_history(char* unused) {}

#else
#include <editline/readline.h>
#include <editline/history.h>
#endif

int main(int argc, char** argv){
    // Print version and exit information
    puts("Lispy Version 0.0.0.0.1");
    puts("Press Ctrl+c to exit\n"); 

    // REPL (Read Eval Print Loop)
    while (1) {
        // output prompt
        char* input = readline("lispy> ");
        add_history(input);

        // Echo input back to user
        printf("No you're a %s\n", input);

        free(input);
    }

    return 0; 
}