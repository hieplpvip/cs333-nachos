#include "syscall.h"
#include "copyright.h"

int main(int argc, char **argv) {
  int fd;
  int size;
  int i;
  char tmp;

  if (argc < 2) {
    PrintString("Usage: cat <filename>\n");
    return 0;
  }

  fd = Open(argv[1], MODE_READ);
  if (fd == -1) {
    PrintString("Failed to open file ");
    PrintString(argv[1]);
    PrintString("\n");
    return 0;
  }

  // Seek to EOF to get file size
  size = Seek(-1, fd);

  // Seek back to beginning and read file
  Seek(0, fd);
  for (i = 0; i < size; i++) {
    Read(&tmp, 1, fd);
    Write(&tmp, 1, ConsoleOutputId);
  }

  Close(fd);

  return 0;
}
