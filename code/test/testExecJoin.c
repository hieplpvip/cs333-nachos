#include "syscall.h"
#include "libc.h"

int main() {
  SpaceId newProc1;
  SpaceId newProc2;
  char* argv1[9];
  char* argv2[9];

  argv1[0] = "copy";
  argv1[1] = "copy.c";
  argv1[2] = "hello.c";

  argv2[0] = "cat";
  argv2[1] = "copy.c";

  newProc1 = ExecV(3, argv1);
  newProc2 = ExecV(2, argv2);
  Join(newProc1);
  Join(newProc2);

  argv1[0] = "cat";
  argv1[1] = "hello.c";
  newProc1 = ExecV(2, argv1);
  Join(newProc1);

  argv1[0] = "delete";
  argv1[1] = "hello.c";
  newProc1 = ExecV(2, argv1);
  Join(newProc1);

  return 0;
}
