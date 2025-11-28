#include <stdio.h>
#include <stdlib.h>

#include "mpc.h"

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
    // Create parsers
    mpc_parser_t* Number    = mpc_new("number");
    mpc_parser_t* Operator  = mpc_new("operator");
    mpc_parser_t* Expr      = mpc_new("expr");
    mpc_parser_t* Lispy     = mpc_new("lispy");

    // Define language
    mpca_lang(MPCA_LANG_DEFAULT, 
        "                                                           \
            number      : /-?[0-9]+/;                               \
            operator    : '+' | '-' | '*' | '/' |;                  \
            expr        : <number> | '(' <operator> <expr>+ ')';    \
            lispy       : /^/ <operator> <expr>+ /$/;               \
        ", 
    Number, Operator, Expr, Lispy);


    // Print version and exit information
    puts("Lispy Version 0.0.0.0.1");
    puts("Press Ctrl+c to exit\n"); 

    // REPL (Read Eval Print Loop)
    while (1) {
        // output prompt
        char* input = readline("lispy> ");
        add_history(input);

        // Attempt to parse user input
        mpc_result_t r; 
        if(mpc_parse("<stdin>", input, Lispy, &r)){
            // On success print ast
            mpc_ast_print(r.output); 
            mpc_ast_delete(r.output); 
        } else {
            // On error print message
            mpc_err_print(r.error); 
            mpc_err_delete(r.error);
        }

        free(input);
    }

    // undefine and delete parser
    mpc_cleanup(4, Number, Operator, Expr, Lispy);

    return 0; 
}