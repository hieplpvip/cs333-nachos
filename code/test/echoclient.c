#include "syscall.h"
#include "copyright.h"

int main() {
  int fd[4], i;
  char *buffer[4];
  char retbuffer[4][50];

  buffer[0] = "hello world";
  buffer[1] = "meow";
  buffer[2] = "wolf";
  buffer[3] = "caw";

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
    Write(buffer[i], sizeof(buffer[i]), fd[i]);
  }

  // Receive data and print
  for (i = 0; i < 4; i++) {
    Read(retbuffer[i], sizeof(buffer[i]), fd[i]);
    retbuffer[i][sizeof(buffer[i])] = '\0';
    PrintString(retbuffer[i]);
    PrintString("\n");
  }

  // Close sockets
  for (i = 0; i < 4; i++) {
    CloseTCP(fd[i]);
  }

  return 0;
}
