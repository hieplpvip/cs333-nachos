#include "syscall.h"
#include "copyright.h"

int main() {
  int fd[4], i;
  char *buffer[4], *retbuffer[4];
  buffer[0] = "hello world";
  buffer[1] = "meow";
  buffer[2] = "wolf";
  buffer[3] = "caw";
  for (i = 0; i < 4; i++)
    fd[i] = SocketTCP();
  for (i = 0; i < 4; i++)
    Connect(fd[i], "127.0.0.1", 8080);
  for (i = 0; i < 4; i++)
    Send(fd[i], buffer[i], 1024);
  for (i = 0; i < 4; i++)
    Receive(fd[i], retbuffer[i], 1024);
  for (i = 0; i < 4; i++)
    CloseTCP(fd[i]);

  return 0;
}
