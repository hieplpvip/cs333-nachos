#include "syscall.h"
#include "libc.h"

int main(int argc, char** argv) {
  int i;
  i = 0;
  while (i < 10) {
    if (strcmp(argv[1], "true") == 0) {
      Wait("test");
    }
    PrintString("test0");
    PrintString("test1");
    PrintString("test2");
    PrintString("test3");
    PrintString("\n");
    if (strcmp(argv[1], "true") == 0) {
      Signal("test");
    }
    ++i;
  }
  return 0;
}
