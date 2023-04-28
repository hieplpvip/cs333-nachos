#include "syscall.h"
#include "copyright.h"
#include "libc.h"

int main() {
  int fd[4], i;
  char* buffer[4];
  char retbuffer[4][50];

  buffer[0] = "hello World,";
  buffer[1] = "My name is";
  buffer[2] = "nachos";
  buffer[3] = "from CS333!";

  // Create sockets
  for (i = 0; i < 4; i++) {
    fd[i] = SocketTCP();
  }

  // Connect to server
  for (i = 0; i < 4; i++) {
    Connect(fd[i], "127.0.0.1", 8080);
  }

  // Send data to server
  for (i = 0; i < 4; i++) {
    Write(buffer[i], strlen(buffer[i]) + 1, fd[i]);
    PrintString("Sent: ");
    PrintString(buffer[i]);
    PrintString("\n");
  }

  // Receive data and print
  for (i = 0; i < 4; i++) {
    Read(retbuffer[i], strlen(buffer[i]) + 1, fd[i]);
    retbuffer[i][strlen(buffer[i])] = '\0';
    PrintString("Received: ");
    PrintString(retbuffer[i]);
    PrintString("\n");
  }

  // Close sockets
  for (i = 0; i < 4; i++) {
    CloseTCP(fd[i]);
  }

  return 0;
}
