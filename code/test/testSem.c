#include "syscall.h"
#include "libc.h"

int main(int argc, char** argv) {
  SpaceId test1;
  SpaceId test2;
  char* argv1[2];
  char* argv2[2];

  CreateSemaphore("test", 1);

  argv1[0] = "testSemChild1";
  argv1[1] = argv[1];
  argv2[0] = "testSemChild2";
  argv2[1] = argv[1];

  if (strcmp(argv[1], "true") == 0) {
    PrintString("\nHas semaphore\n");
  } else {
    PrintString("\nNo semaphore\n");
  }
  test1 = ExecV(2, argv1);
  test2 = ExecV(2, argv2);

  Join(test1);
  Join(test2);

  return 0;
}
