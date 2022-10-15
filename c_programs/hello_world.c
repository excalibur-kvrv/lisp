#include <stdio.h>

int main(int argc, char** argv) {

  // for loop printing hello world 5 times
  for (int i = 0; i < 5; i++) {
    puts("Hello world!: for");
  }

  int i = 0;
  while (i < 5) {
    puts("Hello world: while");
    i++;
  }

  puts("Hello world");
  return 0;
}