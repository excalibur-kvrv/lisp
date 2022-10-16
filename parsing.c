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


int main(int argc, char** argv) {
  // Create some parsers
  mpc_parser_t* Number = mpc_new("number");
  mpc_parser_t* Operator = mpc_new("operator");
  mpc_parser_t* Expression = mpc_new("expression");
  mpc_parser_t* Program = mpc_new("program");

  mpca_lang(MPCA_LANG_DEFAULT, 
  "                                                        \
    number     : /-?[0-9]+(\\.[0-9]+)?/ ;                              \
    operator   : '+' | '-' | '*' | '/' ;                   \
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
      mpc_ast_print(r.output);
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