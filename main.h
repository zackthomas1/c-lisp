#ifndef MAIN_H
#define MAIN_H

#include "mpc.h"

// create enumeration of possible lval types 
enum { LVAL_ERR, LVAL_NUM, LVAL_SYM, LVAL_SEXPR };

typedef struct lval{ 
    int type;
    long num;

    char* err;
    char* sym;

    // pointer to a list of lval*
    int count; 
    struct lval** cell;
} lval;

// Create new lval
lval* lval_num(long x);
lval* lval_sym(char* s);
lval* lval_sexpr(void);
lval* lval_err(char* msg);

// free 
void free_lval(lval* v);

// create lval from abstract syntax tree (ast).
lval* lval_read_num(mpc_ast_t* t);
lval* lval_read(mpc_ast_t* t);
lval* lval_add(lval* v, lval* x);

// 
void lval_print(lval* v);
void lval_println(lval* v);
void lval_expr_print(lval* v, char open, char close);

//
int number_of_nodes(mpc_ast_t* t);
int number_of_leaves(mpc_ast_t* t);

//
lval* lval_pop(lval* v, int index);
lval* lval_take(lval* v, int index);

//
lval* lval_eval_sexpr(lval* v);
lval* builtin_op(lval* a, char* op);
lval* lval_eval(lval* v);

//
lval* eval_op(lval* x, char* op, lval* y);
lval* eval(mpc_ast_t* t);

#endif