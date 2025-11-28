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

int number_of_nodes(mpc_ast_t* t) {
    // base case
    if (t->children_num == 0) { return 1;}

    // recursive step
    int total_nodes = 1;
    for (int i = 0; i < t->children_num; i++){
        total_nodes += number_of_nodes(t->children[i]);
    } 
    return total_nodes;
}

long eval_op(long x, char* op, long v){
    if(strcmp("+", op) == 0) { return x + v; }
    if(strcmp("-", op) == 0) { return x - v; }
    if(strcmp("*", op) == 0) { return x * v; }
    if(strcmp("/", op) == 0) { return x / v; }
    return 0;
}

long eval(mpc_ast_t* t) {
    // if tagged as number return value directly
    if(strstr(t->tag, "number")) { return atoi(t->contents); }

    // select operator (second child)
    char* op = t->children[1]->contents;

    // store third child as variable 'x'
    long x = eval(t->children[2]); 

    // iterator the remain children combining them
    for(int i = 3; i < t->children_num; i++){
        // if 'expr' is not a substr of tag continue to next child
        if(strstr(t->children[i]->tag, "expr") == 0) {continue; }
        
        x = eval_op(x, op, eval(t->children[i]));
    }

    return x;
}



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

            // load AST from output
            mpc_ast_t* a = r.output; 

            // output retult of evaluation of ast
            long res = eval(a); 
            printf("Evaluation: %li\n", res); 

            // output number of nodes in abstract syntax tree
            printf("Number of nodes: %i\n", number_of_nodes(a));

            // output root node info
            printf("Tag: %s\n", a->tag);
            printf("Contents: %s\n", a->contents);
            printf("Number of children: %i\n", a->children_num);

            // Get first child and output info
            mpc_ast_t* c0 = a->children[0]; 
            printf("First Child Tag: %s\n", c0->tag);
            printf("First Child Contents: %s\n", c0->contents);
            printf("First Child number of children: %i\n", c0->children_num);
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