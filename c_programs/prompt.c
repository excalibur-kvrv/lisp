#include <stdio.h>
#include <stdlib.h>

// If we are compiling on windows complete these functions
#ifdef _WIN32
#include <string.h>

static char buffer[2048];

// fake readline function
char* readline(char* prompt) {
  fputs(prompt, stdout);
  fgets(buffer, 2048, stdin);
  char* cpy = malloc(strlen(buffer) + 1);
  strcpy(cpy, buffer);
  cpy[strlen(cpy) - 1] = '\0';
  return cpy;
}

// Fake add history function
void add_history(char* unused) {}

// Otherwise include the editline headers
#else
#include <editline/readline.h>
#include <editline/history.h>
#endif

int main(int argc, char** argv) {
  puts("Lispy version 0.0.0.0.1");
  puts("Press ctrl+c to exit");
  
  while(1) {
    // output our prompt and get input
    char* input = readline("Lispy> ");

    // Add input to history
    add_history(input);

    // print input back to user
    printf("No you are a %s\n", input);

    // Free retrieved input
    free(input); 
  }


  return 0;
}