/* add.c
 *	Simple program to test whether the system call interface works.
 *
 *	Just do a add syscall that adds two values and returns the result.
 *
 */

#include "syscall.h"

int main() {
  int result;

  result = Add(42, 23);

  return 0;
}
