#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#define number 448          // system call table entry number

long main(int argc, char **argv) {
  if (argc < 3) exit(-1);

  int a = atoi(argv[1]);    // multiplicand
  int b = atoi(argv[2]);    // multiplier
  long c;                   // product

  long res = syscall(number, a, b, &c);

  printf("%d * %d = %ld\n", a, b, c);
  exit(res);
}
