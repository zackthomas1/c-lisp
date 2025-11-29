#ifndef MAIN_H
#define MAIN_H

#include "mpc.h"

#define LASSERT(args, cond, err) if (!(cond)) { free_lval(args); return lval_err(err); }

struct lval; 
struct lenv;
typedef struct lval lval;
typedef struct lenv lenv;

// create enumeration of possible lval types 
enum { LVAL_NUM, LVAL_SYM, LVAL_ERR,
       LVAL_FUN, LVAL_SEXPR, LVAL_QEXPR };

typedef lval* (*lbuiltin)(lenv*, lval*);

struct lval{ 
    int type;

    long num;
    char* err;
    char* sym;
    lbuiltin fun;

    // pointer to a list of lval*
    int count; 
    lval** cell;
}; 

struct lenv {
    int count; 
    char** syms; 
    lval** vals;
};

// lenv alllocation/deallocation
lenv* lenv_new(void);
void free_lenv(lenv* e);

// lenv methods
lval* lenv_get(lenv* e, lval* k);
void lenv_put(lenv* e, lval* k, lval* v);

// lval alllocation/deallocation
lval* lval_num(long x);
lval* lval_sym(char* s);
lval* lval_err(char* msg);
lval* lval_fun(lbuiltin func);
lval* lval_sexpr(void);
lval* lval_qexpr(void);
void free_lval(lval* v);

// lval methods
// create lval from abstract syntax tree (ast).
lval* lval_add(lval* v, lval* x);
lval* lval_copy(lval* v);
lval* lval_cons(lval* x, lval* y);

// 
void lval_print(lval* v);
void lval_println(lval* v);
void lval_expr_print(lval* v);

/**
 * @brief extracts single element from s-expression at index 
 * and shifts the rest of the list backward so that it no longer contains 
 * that lval* 
 * 
 * @param s-expression
 * @param index
 * @return the element at index in s-expression v
 */
lval* lval_pop(lval* v, int index);
/**
 * @brief deletes the list it has extracted the element from.
 * This is like taking an element from the list and deleting the rest.
 * 
 * @param s-expression
 * @param index
 * @return the element at index in s-expression v
 */
lval* lval_take(lval* v, int index);

//
lval* lval_eval_sexpr(lenv* e, lval* v);
lval* lval_eval(lenv* e, lval* v);

//
void lenv_add_builtin(lenv* e, char* name, lbuiltin func);
void lenv_add_builtins(lenv* e);

lval* builtin_add(lenv* e, lval* a);
lval* builtin_sub(lenv* e, lval* a);
lval* builtin_mul(lenv* e, lval* a);
lval* builtin_div(lenv* e, lval* a);

lval* builtin_set(lenv* e, lval* a);
lval* builtin_setq(lenv* e, lval* a);
lval* builtin_car(lenv* e, lval* a);
lval* builtin_cdr(lenv* e, lval* a);
lval* builtin_cons(lenv* e, lval* a);
lval* builtin_eval(lenv* e, lval* a);

lval* builtin_op(lenv* e, lval* a, char* op);

// ast evaluation methods
lval* lval_read_num(mpc_ast_t* t);
lval* lval_read(mpc_ast_t* t);
int number_of_nodes(mpc_ast_t* t);
int number_of_leaves(mpc_ast_t* t);

lval* eval_op(lval* x, char* op, lval* y);
lval* eval(mpc_ast_t* t);

#endif