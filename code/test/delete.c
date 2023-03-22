#include "syscall.h"
#include "copyright.h"

int main(int argc, char **argv) {
  if (argc < 2) {
    PrintString("Usage: delete <filename>\n");
    return 0;
  }
  if (Remove(argv[1]) == -1) {
    PrintString("Failed to delete file ");
    PrintString(argv[1]);
    PrintString("\n");
  } else {
    PrintString("Successfully deleted file ");
    PrintString(argv[1]);
    PrintString("\n");
  }
  return 0;
}
