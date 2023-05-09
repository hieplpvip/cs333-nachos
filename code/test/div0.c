#include "syscall.h"
#include "libc.h"

int main() {
  int result;

  result = 1 / 0;

  Write("ah", 2, ConsoleOutputId);
  return 0;
}
