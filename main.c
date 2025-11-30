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

lenv* lenv_new(void) {
    lenv* e = malloc(sizeof(lenv));
    e->count = 0; 
    e->syms = NULL;
    e->vals = NULL;
    return e;
}

void free_lenv(lenv* e) {
    for (int i = 0; i < e->count; i++) {
        free(e->syms[i]);
        free_lval(e->vals[i]);
    }

    free(e->syms); 
    free(e->vals);
    free(e);
}

lval* lenv_get(lenv* e, lval* k) {
    // Iterate over all items in enironment
    for(int i = 0; i < e->count; i++) {
        // check if store symbol string matches k symbol
        // if it does return a copyof the value
        if(strcmp(e->syms[i], k->sym) == 0){
            return lval_copy(e->vals[i]);
        }
    }
    // If no symbol found return error
    return lval_err("unbound symbol '%s'", k->sym);
}

void lenv_put(lenv* e, lval* k, lval* v) {
    // iterate over all items in env to check if variable already exists
    for (int i = 0; i < e->count; i++){

        // if variable found delete item at that position
        // and replace parameter lval
        if(strcmp(e->syms[i], k->sym) == 0){
            free_lval(e->vals[i]); 
            e->vals[i] = lval_copy(v);
            return;
        }
    }

    // if no existing entry found allocate space
    e->count++; 
    e->syms = realloc(e->syms, sizeof(char*) * e->count);
    e->vals = realloc(e->vals, sizeof(lval*) * e->count);

    // copy content of lval and symbol string into new location
    e->syms[e->count-1] = malloc(strlen(k->sym) + 1); 
    strcpy(e->syms[e->count-1], k->sym);
    e->vals[e->count-1] = lval_copy(v); 
}

lval* lval_super() {
    lval* v = malloc(sizeof(lval));
    v->type = LVAL_NUM;

    v->num = INT_MIN;
    v->err = NULL;
    v->sym = NULL;

    v->fun = NULL;
    v->env = NULL;
    v->formals = NULL;
    v->body = NULL;

    v->count = 0;
    v->cell = NULL;
    return v;
}


// Create a new number type lval
lval* lval_num(long x) { 
    lval* v = lval_super();

    v->type = LVAL_NUM; 
    v->num = x;

    return v;
}

lval* lval_sym(char* s) {
    lval* v = lval_super();

    v->type = LVAL_SYM;
    v->sym = malloc(strlen(s) + 1);
    strcpy(v->sym, s);

    return v;
}

// Create a new error type lval
lval* lval_err(char* fmsg, ...){ 
    lval* v = lval_super();
    v->type = LVAL_ERR;

    // Create a va and initialize 
    va_list va; 
    va_start(va, fmsg);

    v->err = malloc(512);
    
    // printf the error string with max of 511 chars
    vsnprintf(v->err, 511, fmsg, va);

    // reallocate number of bytees actually used
    v->err = realloc(v->err, strlen(v->err)+1);

    // cleanup va list 
    va_end(va);

    return v;
}

lval* lval_fun(lbuiltin func) {
    lval* v = lval_super();
    
    v->type = LVAL_FUN; 
    v->fun = func;

    return v;
}

lval* lval_lambda(lval* formals, lval* body) {
    lval* v = lval_super();
    v->type = LVAL_FUN;

    v->fun = NULL;
    v->env = lenv_new();
    v->formals = formals; 
    v->body = body; 

    return v;
}

lval* lval_sexpr(void) {
    lval* v = lval_super();
    
    v->type = LVAL_SEXPR; 
    v->count = 0; 
    v->cell = NULL; // NULL is a special constant that points to memory location 0
    
    return v;
}

lval* lval_qexpr(void) {
    lval* v = lval_super();

    v->type = LVAL_QEXPR;
    v->count = 0; 
    v->cell = NULL;

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
    case LVAL_FUN:
        break;
    case LVAL_QEXPR:
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

lval* lval_add(lval* v, lval* x){ 
    v->count++; 
    v->cell = realloc(v->cell, sizeof(lval*) * v->count);
    v->cell[v->count-1] = x;
    return v;
}

lval* lval_copy(lval* v) { 
    lval* x = malloc(sizeof(lval));
    x->type = v->type;
    
    x->num = INT_MIN;
    x->err = NULL;
    x->sym = NULL;
    x->fun = NULL;
    x->count = 0;
    x->cell = NULL;

    switch (v->type)
    {
    case LVAL_NUM:
        x->num = v->num;
        break;
    case LVAL_SYM:
        x->sym = malloc(strlen(v->sym) + 1);
        strcpy(x->sym, v->sym);
        break;
    case LVAL_ERR:
        x->err = malloc(strlen(v->err) + 1);
        strcpy(x->err, v->err); 
        break;
    case LVAL_FUN:
        x->fun = v->fun;
        break;
    case LVAL_QEXPR:
    case LVAL_SEXPR:
        x->count = v->count; 
        x->cell = malloc(sizeof(lval*) * x->count); 
        for(int i = 0; i < x->count; i++){
            x->cell[i] = lval_copy(v->cell[i]);
        }
        break;
    default:
        break;
    }
    return x;
}

lval* lval_cons(lval* x, lval* y) {
    // for each cell in y add it to x
    while(y->count) {
        lval* yprim = lval_pop(y, 0); 
        if (yprim->type == LVAL_SEXPR){ 
            x = lval_cons(x, yprim);
        }else{
            x = lval_add(x, yprim);
        }
    }

    free_lval(y); 
    return x;
}

char* lval_type(int t) {
    switch (t)
    {
    case LVAL_NUM:
        return "Number";
    case LVAL_SYM:
        return "Number";
    case LVAL_ERR:
        return "Error";
    case LVAL_FUN:
        return "Function";
    case LVAL_SEXPR:
        return "S-Express";
    case LVAL_QEXPR:
        return "Q-Expression";
    default:
        return "Unknown";
    }
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
    case LVAL_FUN:
        printf("<function>");
        break;
    case LVAL_SEXPR:
        putchar('(');
        lval_expr_print(v);
        putchar(')');
        break;
    case LVAL_QEXPR:
        putchar('\'');
        lval_expr_print(v);
        break;
    default:
        break;
    }
}

void lval_expr_print(lval* v){
    for(int i = 0; i < v->count; i++){
        // print value contained within
        lval_print(v->cell[i]);
    
        // don't print a trailing white space characer
        if (i != (v->count-1)){ 
            putchar(' ');
        }
    }
}

void lval_println(lval* v){ 
    lval_print(v);
    putchar('\n');
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

lval* lval_eval_sexpr(lenv* e, lval* v)
{
    // eval children
    for (int i = 0; i < v->count; i++){ 
        v->cell[i] = lval_eval(e, v->cell[i]);

        // error checking
        if (v->cell[i]->type == LVAL_ERR){
            return lval_take(v, i);
        }
    }

    // empty expression
    if (v->count == 0) { return v; }

    // single expression
    if (v->count == 1) { return lval_take(v, 0); }

    // ensure 1st element is function after evaluation
    lval* f = lval_pop(v, 0);
    if(f->type != LVAL_FUN){
        free_lval(f); 
        free_lval(v);
        return lval_err("First element must be a function.");
    }

    // call builtin with operator
    lval* result = f->fun(e, v); 
    free_lval(f); 
    return result;
}

lval* lval_eval(lenv* e, lval* v) {
    
    // Evaluate/resolve symbols using environment map
    if (v->type == LVAL_SYM){
        lval* x = lenv_get(e, v);
        free_lval(v);
        return x;
    }

    // evaluate S-expression
    if (v->type == LVAL_SEXPR) { return lval_eval_sexpr(e, v); }

    // all other lvals evaluate to themself
    return v;
}

void lenv_add_builtin(lenv* e, char* name, lbuiltin func){
    lval* k = lval_sym(name); 
    lval* v = lval_fun(func); 
    lenv_put(e, k, v);
    free_lval(k);
    free_lval(v);
}

void lenv_add_builtins(lenv* e){
    // math operators
    lenv_add_builtin(e, "+", builtin_add);
    lenv_add_builtin(e, "-", builtin_sub);
    lenv_add_builtin(e, "*", builtin_mul);
    lenv_add_builtin(e, "/", builtin_div);
    lenv_add_builtin(e, "set", builtin_set);
    lenv_add_builtin(e, "setq", builtin_setq);
    lenv_add_builtin(e, "car", builtin_car);
    lenv_add_builtin(e, "cdr", builtin_cdr);
    lenv_add_builtin(e, "cons", builtin_cons);
    lenv_add_builtin(e, "eval", builtin_eval);
}

lval* builtin_add(lenv* e, lval* a) {
    return builtin_op(e, a, "+");
}

lval* builtin_sub(lenv* e, lval* a) {
    return builtin_op(e, a, "-");
}

lval* builtin_mul(lenv* e, lval* a) {
    return builtin_op(e, a, "*");
}

lval* builtin_div(lenv* e, lval* a) {
    return builtin_op(e, a, "/");
}

lval* builtin_set(lenv* e, lval* a) {
    LASSERT(a, a->cell[0]->type == LVAL_QEXPR,
        "Function 'set' passed incorrect typefor argument 0. "
            "Got %s, Expected %s.",
            lval_type(a->cell[0]->type), lval_type(LVAL_QEXPR));

    // first argument is symbol
    lval* sym = a->cell[0];

    LASSERT(a, a->count - 1 == 1,
        "Function 'set' Unable to define variable." 
        "Mismatch between symbols to actuals(values)"); 

    lval* val = a->cell[1];

    lenv_put(e, sym->cell[0], val);
    
    free_lval(a);
    return lval_sexpr();
}

lval* builtin_setq(lenv* e, lval* a) {
    return NULL;
}

lval* builtin_car(lenv* e, lval* a) {
    // check error conditions
    LASSERT(a, a->count == 1, 
        "Function 'car' passed too few arguments!"
        "Got %i, Expected %i",
        a->count, 1);
    
    LASSERT(a, a->cell[0]->type == LVAL_QEXPR, 
        "Function 'car' passed incorrect typefor argument 0. "
            "Got %s, Expected %s.",
            lval_type(a->cell[0]->type), lval_type(LVAL_QEXPR));

    lval* v = lval_take(lval_take(a, 0), 0); 
    if(v->type == LVAL_SEXPR){
        return lval_pop(v, 0);
    }
    return v;
}

lval* builtin_cdr(lenv* e, lval* a) {
    // check error conditions
    LASSERT(a, a->count == 1, 
        "Function 'cdr' passed too few arguments!"
        "Got %i, Expected %i",
        a->count, 1);
    
    LASSERT(a, a->cell[0]->type == LVAL_QEXPR, 
        "Function 'cdr' passed incorrect typefor argument 0. "
            "Got %s, Expected %s.",
            lval_type(a->cell[0]->type), lval_type(LVAL_QEXPR));
    
    LASSERT(a, a->cell[0]->count != 0, 
        "Function 'cdr' passed passed {}");

    lval* v = lval_take(lval_take(a, 0), 0);
    if (v->type == LVAL_SEXPR){
        free_lval(lval_pop(v,0));
        return lval_add(lval_qexpr(), v);
    }
    return lval_qexpr();
}

lval* builtin_cons(lenv* e, lval* a){
    
    for (int i = 0; i < a->count; i++) {
        LASSERT(a, a->cell[i]->type == LVAL_QEXPR,
            "Function 'cons' passed incorrect typefor argument 0. "
            "Got %s, Expected %s.",
            lval_type(a->cell[0]->type), lval_type(LVAL_QEXPR));
    }

    lval* x = lval_pop(lval_pop(a, 0), 0);
    if (x->type == LVAL_QEXPR) {
        x->type = LVAL_SEXPR;
    } else if (x->type != LVAL_SEXPR) {
        x = lval_add(lval_sexpr(), x); 
    }

    while (a->count){ 
        x = lval_cons(x, lval_pop(a, 0));
    }

    free_lval(a);
    return lval_add(lval_qexpr(), x);
}

lval* builtin_eval(lenv* e, lval* a) {
    LASSERT(a, a->count == 1, 
        "Function 'eval' passed too may arguments");
    LASSERT(a, a->cell[0]->type == LVAL_QEXPR, 
        "Function 'eval' passed incorrect type");
    
        lval* x = lval_take(a, 0); 
        x->type = LVAL_SEXPR; 
        return lval_eval(e, x);
}

lval* builtin_op(lenv* e, lval* a, char* op){

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
    if(strcmp(t->tag, ">") == 0) { x = lval_sexpr(); t = t->children[0]; }
    else if(strstr(t->tag, "sexpr")) { x = lval_sexpr(); }
    else if(strstr(t->tag, "qexpr")) { x = lval_qexpr(); }

    // fill this list with valid expression contained within
    for(int i = 0; i < t->children_num; i++){
        if(strcmp(t->children[i]->contents, "(") == 0) { continue; }
        if(strcmp(t->children[i]->contents, ")") == 0) { continue; }
        if(strcmp(t->children[i]->contents, "\'") == 0) { continue; }
        if(strcmp(t->children[i]->tag, "regex") == 0) { continue; }
        x = lval_add(x, lval_read(t->children[i]));
    }
    return x;
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
    mpc_parser_t* Qexpr     = mpc_new("qexpr");
    mpc_parser_t* Expr      = mpc_new("expr");
    mpc_parser_t* Lispy     = mpc_new("lispy");

    // Define language
    mpca_lang(MPCA_LANG_DEFAULT, 
        "                                                           \
            number  : /-?[0-9]+/;                                   \
            symbol  : /[a-zA-Z0-9_+\\-*\\/\\\\=<>!&]+/;    \
            sexpr   : '(' <expr>* ')';                              \
            qexpr   : '\''<expr>;                              \
            expr    : <number> | <symbol> | <sexpr> | <qexpr>;                \
            lispy   : <sexpr>;                               \
        ", 
    Number, Symbol, Sexpr, Qexpr, Expr, Lispy);

    // define alist
    lenv* e = lenv_new(); 
    lenv_add_builtins(e);

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

            // // Print ast
            // mpc_ast_print(a); 

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

            // transform ast to sexptr
            lval* x = lval_read(a);
            printf("init lval: "); 
            lval_println(x);

            // output sexpr eval
            lval* res = lval_eval(e, x);
            printf("eval lval: "); 
            lval_println(res);
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
    mpc_cleanup(6, Number, Symbol, Sexpr, Qexpr, Expr, Lispy);

    return 0; 
}