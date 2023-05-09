#include "syscall.h"
#include "libc.h"

int main() {
  SpaceId newProc1;
  SpaceId newProc2;
  char* argv1[2];
  char* argv2[3];

  argv1[0] = "cat";
  argv1[1] = "text.txt";

  argv2[0] = "copy";
  argv2[1] = "text.txt";
  argv2[2] = "a.txt";

  newProc1 = ExecV(2, argv1);  // Project 01
  newProc2 = ExecV(3, argv2);  // Project 01

  Join(newProc1);
  Join(newProc2);
}
