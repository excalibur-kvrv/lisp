#include <stdio.h>
#include <stdlib.h>
#include <editline/readline.h>
#include <editline/history.h>
#include "mpc.h"

/*
Lisp Grammer:-
program       :  /^/ <operator> <expression>+ /$/
expression    :  <number> | "(" <operator> <expression>+ ")"
operator      :  "+" | "-" | "*" | "/" | "%"
number        :  -?[0-9]+(\\.[0-9]+)?

*/

long negate(long x) {
  return -x;
}

// use operator string to see which operation to perform
long eval_op(long x, char* op, long y) {
  if (strcmp(op, "+") == 0) return x + y;
  if (strcmp(op, "-") == 0) return x - y;
  if (strcmp(op, "*") == 0) return x * y;
  if (strcmp(op, "/") == 0) return x / y;
  if (strcmp(op, "%") == 0) return x % y;
  if (strcmp(op, "^") == 0) return (long)pow(x, y);
  return 0;
}

long eval(mpc_ast_t* t) {
  // if tagged as number return it directly
  if (strstr(t -> tag, "number")) {
    return atoi(t -> contents);
  }

  // the operator is always the second child
  char* op = t -> children[1] -> contents;

  // we store the third variable in x
  long x = eval(t -> children[2]);

  // negate 1st operand if there is no 2nd operand
  if ((strcmp(op, "-") == 0) && !(strstr(t -> children[3] -> tag, "expression"))) {
    return negate(x);
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
  "                                                        \
    number     : /-?[0-9]+(\\.[0-9]+)?/ ;                              \
    operator   : '+' | '-' | '*' | '/' | '%' | '^' ;                   \
    expression : <number> | '(' <operator> <expression>+ ')' ;   \
    program    : /^/ <operator> <expression>+ /$/ ;              \
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
      long result = eval(r.output);
      mpc_ast_print(r.output);
      printf("%li\n", result);
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