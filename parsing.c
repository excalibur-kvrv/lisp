#include <stdio.h>
#include <stdlib.h>
#include <editline/readline.h>
#include <editline/history.h>
#include "mpc.h"

/*
Lisp Grammer:-
program       :  /^/ <operator> <expression>+ /$/
expression    :  <number> | "(" <operator> <expression>+ ")"
operator      :  "+" | "-" | "*" | "/" | "%" | "^"
number        :  -?[0-9]+(\\.[0-9]+)?
*/


// Declare new lval struct
typedef struct {
  int type;
  long num;
  int err;
} lval;

// Create enumeration of possible lval types
enum {
  LVAL_NUM,
  LVAL_ERR
};

// Create enumeration of possible error types
enum {
  LERR_DIV_ZERO,
  LERR_BAD_OP,
  LERR_BAD_NUM
};

// Create a new number type lval
lval lval_num(long x) {
  lval v;
  v.type = LVAL_NUM;
  v.num = x;
  return v;
}

// Create a new error type lval
lval lval_err(int x) {
  lval v;
  v.type = LVAL_ERR;
  v.err = x;
  return v;
}


long negate(long x) {
  return -x;
}


// use operator string to see which operation to perform
lval eval_op(lval x, char* op, lval y) {
  // If either value is an error return it
  if (x.type == LVAL_ERR) return x;
  if (y.type == LVAL_ERR) return y;

  // Otherwise do math on number values
  if (strcmp(op, "+") == 0) return lval_num(x.num + y.num);
  if (strcmp(op, "-") == 0) return lval_num(x.num - y.num);
  if (strcmp(op, "*") == 0) return lval_num(x.num * y.num);
  if (strcmp(op, "/") == 0) {
    return y.num == 0 ? lval_err(LERR_DIV_ZERO) : lval_num(x.num / y.num);
  }
  if (strcmp(op, "%") == 0) return lval_num(x.num % y.num);
  if (strcmp(op, "^") == 0) return lval_num((long)pow(x.num, y.num));
  return lval_num(0);
}


void lval_print(lval v) {
  switch(v.type) {
    // in case type is number print it.
    // then break out of the switch
    case LVAL_NUM: printf("%li", v.num); break;

    // In case the type is of error 
    case LVAL_ERR:
      // check what type of error it is and print it.
      if (v.err == LERR_DIV_ZERO) {
        printf("Error: Division by zero");
      }

      if (v.err == LERR_BAD_OP) {
        printf("Error: Invalid operator");
      }

      if (v.err == LERR_BAD_NUM) {
        printf("Error: Invalid number");
      }
      break;
  }
}

// Print an lval followed by a newline
void lval_println(lval v) {
  lval_print(v);
  putchar('\n');
}

lval eval(mpc_ast_t* t) {
  // if tagged as number return it directly 
  if (strstr(t -> tag, "number")) { 
    // Check if there is some error in conversion
    errno = 0;
    long x = strtol(t -> contents, NULL, 10);
    return errno != ERANGE ? lval_num(x): lval_err(LERR_BAD_NUM);
  }

  // the operator is always the second child
  char* op = t -> children[1] -> contents;

  // we store the third variable in x
  lval x = eval(t -> children[2]);

  // negate 1st operand if there is no 2nd operand
  if ((strcmp(op, "-") == 0) && !(strstr(t -> children[3] -> tag, "expression"))) {
    return lval_num(negate(x.num));
  }

  // Iterate remaining children and combining
  int i = 3;
  while (strstr(t -> children[i] -> tag, "expression")) {
    x = eval_op(x, op, eval(t -> children[i]));
    i++;
  } 

  return x;
}


int main(int argc, char** argv) {
  // Create some parsers
  mpc_parser_t* Number = mpc_new("number");
  mpc_parser_t* Operator = mpc_new("operator");
  mpc_parser_t* Expression = mpc_new("expression");
  mpc_parser_t* Program = mpc_new("program");

  mpca_lang(MPCA_LANG_DEFAULT, 
  "                                                                    \
    number     : /-?[0-9]+(\\.[0-9]+)?/ ;                              \
    operator   : '+' | '-' | '*' | '/' | '%' | '^' ;                   \
    expression : <number> | '(' <operator> <expression>+ ')' ;         \
    program    : /^/ <operator> <expression>+ /$/ ;                    \
  ", Number, Operator, Expression, Program);

  puts("Lispy version 0.0.0.0.1");
  puts("Enter ctrl + c to exit");

  while(1) {
    char* input = readline("Lispy> ");
    add_history(input);

    // Attempt to parse user input
    mpc_result_t r;
    if (mpc_parse("<stdin>", input, Program, &r)) {
      // On success print the ast
      lval result = eval(r.output);
      mpc_ast_print(r.output);
      lval_println(result);
      mpc_ast_delete(r.output);
    } else {
      // Otherwise print error
      mpc_err_print(r.error);
      mpc_err_delete(r.error);
    }

    free(input);
  }

  // Undefine and delete parsers
  mpc_cleanup(4, Number, Operator, Expression, Program);
  return 0;
}