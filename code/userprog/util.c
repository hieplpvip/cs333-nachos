#include "util.h"
#include "kernel.h"
#include "machine.h"

char* User2System(int virtAddr, int limit) {
  int i;  // index
  int oneChar;
  char* kernelBuf = NULL;

  kernelBuf = new char[limit + 1];  // need for terminal string
  if (kernelBuf == NULL)
    return kernelBuf;
  memset(kernelBuf, 0, limit + 1);

  // printf("\n Filename u2s:");
  for (i = 0; i < limit; i++) {
    kernel->machine->ReadMem(virtAddr + i, 1, &oneChar);
    kernelBuf[i] = (char)oneChar;
    // printf("%c",kernelBuf[i]);
    if (oneChar == 0)
      break;
  }
  return kernelBuf;
}
/*
Input: - User space address (int)
- Limit of buffer (int)
- Buffer (char[])
Output:- Number of bytes copied (int)
Purpose: Copy buffer from System memory space to User memory space
*/

int System2User(int virtAddr, int len, char* buffer) {
  if (len < 0) return -1;
  if (len == 0) return len;
  int i = 0;
  int oneChar = 0;
  do {
    oneChar = (int)buffer[i];
    kernel->machine->WriteMem(virtAddr + i, 1, oneChar);
    i++;
  } while (i < len && oneChar != 0);
  return i;
}

void incrementPC() {
  printf("\n");
  /* set previous programm counter (debugging only)*/
  kernel->machine->WriteRegister(PrevPCReg, kernel->machine->ReadRegister(PCReg));

  /* set programm counter to next instruction (all Instructions are 4 byte wide)*/
  kernel->machine->WriteRegister(PCReg, kernel->machine->ReadRegister(PCReg) + 4);

  /* set next programm counter for brach execution */
  kernel->machine->WriteRegister(NextPCReg, kernel->machine->ReadRegister(PCReg) + 4);
}
