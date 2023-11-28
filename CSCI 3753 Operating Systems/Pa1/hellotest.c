#include <unistd.h>
#include <stdlib.h>

int main() {
  long int res = syscall(447);
  exit(res);
}
