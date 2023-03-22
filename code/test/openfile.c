#include "syscall.h"
#include "copyright.h"

int main() {
  int fd;
  Create("test.txt");
  fd = Open("test.txt", MODE_READWRITE);
  Write("Hello World!\n", 13, fd);
  Halt();
}
