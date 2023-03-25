#include "syscall.h"
#include "copyright.h"

int main(int argc, char **argv) {
  int fileFd;
  int socketFd;
  int size;
  int i;
  char tmp;

  if (argc < 2) {
    PrintString("Usage: fileclient <filename>\n");
    return 0;
  }

  fileFd = Open(argv[1], MODE_READ);
  if (fileFd == -1) {
    PrintString("Failed to open file ");
    PrintString(argv[1]);
    PrintString("\n");
    return 0;
  }

  // Seek to EOF to get file size
  size = Seek(-1, fileFd);

  // Seek back to beginning
  // Then read file and send to server
  // Receive data from server and print
  Seek(0, fileFd);
  for (i = 0; i < size; i++) {
    socketFd = SocketTCP();
    Connect(socketFd, "127.0.0.1", 8080);
    Read(&tmp, 1, fileFd);
    Write(&tmp, 1, socketFd);
    Read(&tmp, 1, socketFd);
    Write(&tmp, 1, ConsoleOutputId);
    Close(socketFd);
  }

  // Close file
  Close(fileFd);

  return 0;
}
