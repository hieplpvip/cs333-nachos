#include "syscall.h"
#include "libc.h"

int main() {
  SpaceId newProc;
  int i, argc, empty, exitCode;
  char exitCodeChar[3];
  char prompt[2], buffer[101];
  char* argv[10];

  prompt[0] = '$';
  prompt[1] = ' ';

  for (;;) {
    Write(prompt, 2, ConsoleOutputId);

    i = 0;
    argc = 0;
    empty = 1;

    for (; i < 100;) {
      Read(&buffer[i], 1, ConsoleInputId);
      if (buffer[i] == '\n') {
        // ENTER
        buffer[i] = 0;
        break;
      } else if (buffer[i] == ' ' || buffer[i] == '\t') {
        // SPACE or TAB
        if (!empty) {
          buffer[i] = 0;
          empty = 1;
          i++;
        }
      } else {
        // Others
        if (empty) {
          argv[argc++] = &buffer[i];
          empty = 0;
        }
        i++;
      }
    }
    buffer[100] = 0;

    if (argc > 0) {
      if (strcmp(argv[0], "exit") == 0) {
        return 0;
      }

      newProc = ExecV(argc, argv);
      exitCode = Join(newProc);
      if (exitCode != 0) {
        if (exitCode == -1) {
          exitCodeChar[0] = '-';
          exitCodeChar[1] = '-1';
          exitCodeChar[2] = '\0';
        } else {
          exitCodeChar[0] = exitCode / 10 / 10 % 10 + '0';
          exitCodeChar[1] = exitCode / 10 % 10 + '0';
          exitCodeChar[2] = exitCode % 10 + '0';
        }
        Write("Exit Code: ", 11, ConsoleOutputId);
        Write(exitCodeChar, 3, ConsoleOutputId);
        Write("\n", 1, ConsoleOutputId);
      }
    }
  }
}
