#include <stdio.h>
#include <stdlib.h>

#include "main.h"

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

// Create a new number type lval
lval* lval_num(long x) { 
    lval* v = malloc(sizeof(lval));
    v->type = LVAL_NUM; 
    v->num = x; 
    return v;
}

lval* lval_sym(char* s) {
    lval* v = malloc(sizeof(lval));
    v->type = LVAL_SYM;
    v->sym = malloc(strlen(s) + 1);
    strcpy(v->sym, s);
    return v;
}

lval* lval_sexpr(void) {
    lval* v = malloc(sizeof(lval)); 
    v->type = LVAL_SEXPR; 
    v->count = 0; 
    v->cell = NULL; // NULL is a psecial constant that points to memory location 0
    return v;
}

// Create a new error type lval
lval* lval_err(char* msg){ 
    lval* v = malloc(sizeof(lval));
    v->type = LVAL_ERR;
    v->err = malloc(strlen(msg) + 1);
    strcpy(v->err, msg);
    return v;
}

void free_lval(lval* v){
    switch (v->type)
    {
    case LVAL_NUM:
        break;
    case LVAL_ERR:
        free(v->err);
        break;
    case LVAL_SYM:
        free(v->sym);
        break;
    case LVAL_SEXPR:
        // free all elements inside
        for(int i = 0; i < v->count; i++){
            free_lval(v->cell[i]);
        }
        // also free memory allocated to contain pointers
        free(v->cell);
        break;    
    default:
        break;
    }
    // free memory allocated for lval struct itself
    free(v);
}

lval* lval_read_num(mpc_ast_t* t){
    errno = 0; 
    long x = strtol(t->contents, NULL, 10);
    return errno != ERANGE ? lval_num(x) : lval_err("invalid number");
}

lval* lval_read(mpc_ast_t* t){
    // if symbol or number return conversion to that type
    if(strstr(t->tag, "number")) { return lval_read_num(t); }
    if(strstr(t->tag, "symbol")) { return lval_sym(t->contents); }

    // if root (>) or sexpr ten create empty list
    lval* x = NULL;
    if(strcmp(t->tag, ">") == 0) { x = lval_sexpr(); }
    if(strstr(t->tag, "sexpr")) { x = lval_sexpr(); }

    // fill this list with valid expression contained within
    for(int i = 0; i < t->children_num; i++){
        if(strcmp(t->children[i]->contents, "(") == 0) { continue; }
        if(strcmp(t->children[i]->contents, ")") == 0) { continue; }
        if(strcmp(t->children[i]->tag, "regex") == 0) { continue; }
        x = lval_add(x, lval_read(t->children[i]));
    }
    return x;
}

lval* lval_add(lval* v, lval* x){ 
    v->count++; 
    v->cell = realloc(v->cell, sizeof(lval*) * v->count);
    v->cell[v->count-1] = x;
    return v;
}

void lval_print(lval* v){
    switch (v->type)
    {
    case LVAL_NUM:
        printf("%li", v->num);
        break;
    case LVAL_ERR:
        printf("%s", v->err);
        break;
    case LVAL_SYM:
        printf("%s", v->sym);
        break;
    case LVAL_SEXPR:
        lval_expr_print(v, '(', ')');
        break;
    default:
        break;
    }
}

void lval_expr_print(lval* v, char open, char close){
    putchar(open);

    for(int i = 0; i < v->count; i++){
        // print value contained within
        lval_print(v->cell[i]);
    
        // don't print a trailing white space characer
        if (i != (v->count-1)){ 
            putchar(' ');
        }
    }
    putchar(close);
}

void lval_println(lval* v){ 
    lval_print(v);
    putchar('\n');
}

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

// recursive computes number of leaves in ast
int number_of_leaves(mpc_ast_t* t){ 
    // If this node has no children, it's a leaf
    if(t->children_num == 0) { return 1; }

    // If this node has children, it's not a leaf itself
    // Count leaves in all children
    int total_leaf_nodes = 0; 
    for(int i = 0; i < t->children_num; i++){ 
        total_leaf_nodes += number_of_leaves(t->children[i]);
    }

    return total_leaf_nodes;
}

lval* lval_pop(lval* v, int index) {
    // find item at index 
    lval* x = v->cell[index]; 

    // shift memory afer item 
    memmove(&v->cell[index], &v->cell[index+1], sizeof(lval*) * (v->count-index-1));

    // decrease count of items in list
    v->count--; 

    // realllocate memory used
    v->cell = realloc(v->cell, sizeof(lval*) * v->count);
    return x;
}

lval* lval_take(lval* v, int index) {
    lval* x = lval_pop(v, index);
    free_lval(v);
    return x;
}

lval* lval_eval_sexpr(lval* v)
{
    // eval children
    for (int i = 0; i < v->count; i++){ 
        v->cell[i] = lval_eval(v->cell[i]);
    }

    // error checking
    for (int i = 0; i < v->count; i++){
        if (v->cell[i]->type == LVAL_ERR){
            return lval_take(v, i);
        }
    }

    // empty expression
    if (v->count == 0) { return v; }

    // single expression
    if (v->count == 1) { return lval_take(v, 0); }

    // ensure 1st element is symbol
    lval* f = lval_pop(v, 0);
    if(f->type != LVAL_SYM){
        free_lval(f); 
        free_lval(v);
        return lval_err("S-epxression missing start symbol");
    }

    // call builtin with operator
    lval* result = builtin_op(v, f->sym);
    free_lval(f); 
    return result;
}

lval* builtin_op(lval* a, char* op){

    // check that all arguments are numeric
    for (int i = 0; i < a->count; i++){
        if(a->cell[i]->type != LVAL_NUM){
            free_lval(a);
            return lval_err("Illegal non-numeric operand.");
        }
    }

    // pop first element
    lval* x = lval_pop(a, 0); 

    // if no argument and sub then preform unary negation
    if ((strcmp(op, "-") == 0) && a->count == 0) { 
        x->num = -x->num;
    }

    // while there more elements remaining
    while (a->count > 0){ 
        // pop next element
        lval* y = lval_pop(a, 0);
    
        if (strcmp("+", op) == 0) { x->num += y->num; }
        if (strcmp("-", op) == 0) { x->num -= y->num; }
        if (strcmp("*", op) == 0) { x->num *= y->num; }
        if (strcmp("/", op) == 0) { 
            if (y->num == 0 ) {
                free_lval(x); 
                free_lval(y);
                return lval_err("division by zero");
            }
            x->num /= y->num; 
        }
        free_lval(y);
    }
    free_lval(a);
    return x;
}

lval* lval_eval(lval* v) {
    // evaluate S-expression
    if (v->type == LVAL_SEXPR) { return lval_eval_sexpr(v); }
    return v;
}

lval* eval_op(lval* x, char* op, lval* y){
    // if either of the valuies is an error return immediately
    if(x->type == LVAL_ERR && y->type == LVAL_ERR) { return lval_err("bad number"); }
    if(x->type == LVAL_ERR) { return y; }
    if(y->type == LVAL_ERR) { return x; }

    if(strcmp("+", op) == 0) { return lval_num(x->num + y->num); }
    if(strcmp("-", op) == 0) { return lval_num(x->num - y->num); }
    if(strcmp("*", op) == 0) { return lval_num(x->num * y->num); }
    if(strcmp("/", op) == 0) { 
        if(y->num == 0){ return lval_err("divide by zero"); }
        return lval_num(x->num / y->num); 
    }
    if(strcmp("min", op) == 0) { return ((x->num) > (y->num)) ? lval_num(y->num): lval_num(x->num); }
    if(strcmp("max", op) == 0) { return ((x->num) > (y->num)) ? lval_num(x->num): lval_num(y->num); }

    return lval_err("bad operation");
}

lval* eval(mpc_ast_t* t) {
    // if tagged as number return value directly
    if(strstr(t->tag, "number")) { 
        // check for error in conversion
        errno = 0; 
        long x = strtol(t->contents, NULL, 10); 
        return errno != ERANGE ? lval_num(x) : lval_err("bad number");
    }

    // select operator (second child)
    char* op = t->children[1]->contents;

    // store third child as variable 'x'
    lval* x = eval(t->children[2]); 

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
    mpc_parser_t* Symbol    = mpc_new("symbol");
    mpc_parser_t* Sexpr     = mpc_new("sexpr");
    mpc_parser_t* Expr      = mpc_new("expr");
    mpc_parser_t* Lispy     = mpc_new("lispy");

    // Define language
    mpca_lang(MPCA_LANG_DEFAULT, 
        "                                                           \
            number  : /-?[0-9]+/;                                   \
            symbol  : '+' | '-' | '*' | '/' | \"min\" | \"max\";    \
            sexpr   : '(' <expr>* ')';                              \
            expr    : <number> | <symbol> | <sexpr>;                \
            lispy   : /^/ <expr>* /$/;                               \
        ", 
    Number, Symbol, Sexpr, Expr, Lispy);

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

            // load AST from output
            mpc_ast_t* a = r.output; 

            // On success print ast
            mpc_ast_print(a); 

            // // output number of nodes in abstract syntax tree
            // printf("Number of nodes: %i\n", number_of_nodes(a));
            // printf("Number of leavees: %i\n", number_of_leaves(a));

            // // output root node info
            // printf("Tag: %s\n", a->tag);
            // printf("Contents: %s\n", a->contents);
            // printf("Number of children: %i\n", a->children_num);

            // // Get first child and output info
            // mpc_ast_t* c0 = a->children[0]; 
            // printf("First Child Tag: %s\n", c0->tag);
            // printf("First Child Contents: %s\n", c0->contents);
            // printf("First Child number of children: %i\n", c0->children_num);

            // print sexpr
            lval* x = lval_read(a);
            printf("sepxr: "); lval_println(x);

            // output sexpr eval
            lval* res = lval_eval(x);
            printf("eval sepxr: "); lval_println(res);
            free_lval(res);

            mpc_ast_delete(a); 
        } else {
            // On error print message
            mpc_err_print(r.error); 
            mpc_err_delete(r.error);
        }

        free(input);
    }

    // undefine and delete parser
    mpc_cleanup(5, Number, Symbol, Sexpr, Expr, Lispy);

    return 0; 
}