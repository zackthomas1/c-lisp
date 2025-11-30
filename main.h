#ifndef MAIN_H
#define MAIN_H

#include "mpc.h"

#define LASSERT(args, cond, fmsg, ...) if (!(cond)) { lval* err =  lval_err(fmsg, ##__VA_ARGS__); free_lval(args); return err; }
#define LASSERT_TYPE(func, args, index, expect) \
  LASSERT(args, args->cell[index]->type == expect, \
    "Function '%s' passed incorrect type for argument %i. " \
    "Got %s, Expected %s.", \
    func, index, lval_type(args->cell[index]->type), lval_type(expect))

#define LASSERT_NUM(func, args, num) \
  LASSERT(args, args->count == num, \
    "Function '%s' passed incorrect number of arguments. " \
    "Got %i, Expected %i.", \
    func, args->count, num)

#define LASSERT_NOT_EMPTY(func, args, index) \
  LASSERT(args, args->cell[index]->count != 0, \
    "Function '%s' passed {} for argument %i.", func, index);

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

    // basic
    long num;
    char* err;
    char* sym;
    
    // function
    lbuiltin fun;
    lenv* env;
    lval* formals;
    lval* body;

    // expression
    int count; 
    lval** cell;
}; 

struct lenv {
    lenv* par;
    int count; 
    char** syms; 
    lval** vals;
};

// lval Contructor/Destructor
lenv* lenv_new(void);
void free_lenv(lenv* e);
lenv* lenv_copy(lenv* e);

// lenv methods
lval* lenv_get(lenv* e, lval* k);
void lenv_put(lenv* e, lval* k, lval* v);
void lenv_def(lenv* e, lval*k, lval* v);

// lval Contructor/Destructor
lval* lval_super();
lval* lval_num(long x);
lval* lval_sym(char* s);
lval* lval_err(char* fmsg, ...);
lval* lval_fun(lbuiltin func);
lval* lval_lambda(lval* formals, lval* body);
lval* lval_sexpr(void);
lval* lval_qexpr(void);
void free_lval(lval* v);

// lval methods
// create lval from abstract syntax tree (ast).
lval* lval_add(lval* v, lval* x);
lval* lval_copy(lval* v);
lval* lval_cons(lval* x, lval* y);

// 
char* lval_type(int t);
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
lval* lval_call(lenv* e, lval* f, lval* a);

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
lval* builtin_lambda(lenv* e, lval* a);
lval* builtin_def(lenv* e, lval* a);
lval* builtin_put(lenv* e, lval* a);
lval* builtin_var(lenv* e, lval* a, char* func);

lval* builtin_op(lenv* e, lval* a, char* op);

// ast evaluation methods
lval* lval_read_num(mpc_ast_t* t);
lval* lval_read(mpc_ast_t* t);
int number_of_nodes(mpc_ast_t* t);
int number_of_leaves(mpc_ast_t* t);

lval* eval_op(lval* x, char* op, lval* y);
lval* eval(mpc_ast_t* t);

#endif