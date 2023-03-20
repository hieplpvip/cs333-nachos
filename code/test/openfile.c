#include "syscall.h"
#include "copyright.h"

int main() {
  int fd;
  Create("test.txt");             // test createFile
  fd = Open("test.txt", 0);       // test openFile 0:Read or Write   1: Read-only
  Write("Hello World!", 12, fd);  // test write
  Halt();
}
