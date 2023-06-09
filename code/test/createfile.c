#include "syscall.h"
#include "copyright.h"

int main(int argc, char **argv) {
  if (argc < 2) {
    PrintString("Usage: createfile <filename>\n");
    return 0;
  }
  if (Create(argv[1]) == -1) {
    PrintString("Failed to create file ");
    PrintString(argv[1]);
    PrintString("\n");
  } else {
    PrintString("Successfully created file ");
    PrintString(argv[1]);
    PrintString("\n");
  }
  return 0;
}
