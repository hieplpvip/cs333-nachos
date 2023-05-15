#include "syscall.h"
#include "libc.h"

int main(int argc, char** argv) {
  int i;
  i = 0;
  while (i < 10) {
    if (strcmp(argv[1], "true") == 0) {
      Wait("test");
    }
    PrintString("TEST0");
    PrintString("TEST1");
    PrintString("TEST2");
    PrintString("TEST3");
    PrintString("\n");
    PrintString("\n");
    if (strcmp(argv[1], "true") == 0) {
      Signal("test");
    }
    ++i;
  }
  return 0;
}
