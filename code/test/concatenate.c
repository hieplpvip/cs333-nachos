#include "syscall.h"
#include "copyright.h"

int main(int argc, char **argv) {
  int fd;
  int size;
  int i, j;
  char tmp;

  if (argc < 3) {
    PrintString("Usage: concatenate <first> <second> ...\n");
    return 0;
  }

  for (i = 1; i < argc; i++) {
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
    for (j = 0; j < size; j++) {
      Read(&tmp, 1, fd);
      Write(&tmp, 1, ConsoleOutputId);
    }

    Close(fd);
  }

  return 0;
}
