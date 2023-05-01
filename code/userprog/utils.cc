// utils.cc

#include "utils.h"
#include "main.h"

void RawUser2System(int virtAddr, int size, char* buffer) {
  int tmp;
  for (int i = 0; i < size; i++) {
    kernel->machine->ReadMem(virtAddr + i, 1, &tmp);
    buffer[i] = (char)tmp;
  }
}

int StringUser2System(int virtAddr, int limit, char* buffer) {
  memset(buffer, 0, limit + 1);

  int tmp;
  for (int i = 0; i < limit; i++) {
    kernel->machine->ReadMem(virtAddr + i, 1, &tmp);
    buffer[i] = (char)tmp;
    if (tmp == 0) {
      return i;
    }
  }

  return limit;
}

char* StringUser2System(int virtAddr) {
  int tmp;
  int len = 0;

  do {
    kernel->machine->ReadMem(virtAddr + len, 1, &tmp);
    ++len;
  } while (tmp != 0);

  char* buffer = new char[len];
  for (int i = 0; i < len; i++) {
    kernel->machine->ReadMem(virtAddr + i, 1, &tmp);
    buffer[i] = (char)tmp;
  }
  return buffer;
}

void RawSystem2User(int virtAddr, int size, char* buffer) {
  for (int i = 0; i < size; i++) {
    kernel->machine->WriteMem(virtAddr + i, 1, (int)buffer[i]);
  }
}

void advancePC() {
  // Adapted from `machine/mipssim.cc`, line 682
  int pcAfter = kernel->machine->ReadRegister(NextPCReg);
  kernel->machine->WriteRegister(PrevPCReg, kernel->machine->ReadRegister(PCReg));
  kernel->machine->WriteRegister(PCReg, pcAfter);
  kernel->machine->WriteRegister(NextPCReg, pcAfter + 4);
}

void setReturnCodeAndAdvancePC(int returnCode) {
  kernel->machine->WriteRegister(2, returnCode);
  advancePC();
}
