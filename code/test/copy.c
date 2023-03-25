#include "syscall.h"
#include "copyright.h"

int strcmp(const char *str1, const char *str2) {
  int i;
  for (i = 0; str1[i] || str2[i]; i++) {
    if (!str1[i]) {
      return -1;
    }
    if (!str2[i]) {
      return 1;
    }
    if (str1[i] < str2[i]) {
      return -1;
    }
    if (str1[i] > str2[i]) {
      return 1;
    }
  }
  return 0;
}

int main(int argc, char **argv) {
  int src_fd;
  int dest_fd;
  int size;
  int i;
  char tmp;

  if (argc < 3) {
    PrintString("Usage: copy <src> <dest>\n");
    return 0;
  }

  if (strcmp(argv[1], argv[2]) == 0) {
    PrintString(argv[1]);
    PrintString(" and ");
    PrintString(argv[2]);
    PrintString(" are the same file\n");
    return 0;
  }

  src_fd = Open(argv[1], MODE_READ);
  if (src_fd == -1) {
    PrintString("Failed to open source file ");
    PrintString(argv[1]);
    PrintString("\n");
    return 0;
  }

  Remove(argv[2]);

  if (Create(argv[2]) == -1) {
    PrintString("Failed to create dest file ");
    PrintString(argv[2]);
    PrintString("\n");
  }

  dest_fd = Open(argv[2], MODE_READWRITE);
  if (dest_fd == -1) {
    PrintString("Failed to open dest file ");
    PrintString(argv[2]);
    PrintString("\n");
    return 0;
  }

  // Seek to EOF to get file size
  size = Seek(-1, src_fd);

  // Seek back to beginning and copy
  Seek(0, src_fd);
  Seek(0, dest_fd);
  for (i = 0; i < size; i++) {
    Read(&tmp, 1, src_fd);
    Write(&tmp, 1, dest_fd);
  }

  Close(src_fd);
  Close(dest_fd);

  PrintString("Successfully copied file ");
  PrintString(argv[1]);
  PrintString(" to ");
  PrintString(argv[2]);
  PrintString("\n");

  return 0;
}
