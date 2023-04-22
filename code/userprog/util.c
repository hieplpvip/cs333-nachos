#include "util.h"
#include "kernel.h"
#include "machine.h"

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
  if (buffer == NULL) {
    return NULL;
  }

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

void incrementPC() {
  // Adapted from `machine/mipssim.cc`, line 682

  // Set previous program counter (debugging only)
  kernel->machine->WriteRegister(PrevPCReg, kernel->machine->ReadRegister(PCReg));

  // Set program counter to next instruction (all Instructions are 4 byte wide)
  kernel->machine->WriteRegister(PCReg, kernel->machine->ReadRegister(PCReg) + 4);

  // Set next program counter for branch execution
  kernel->machine->WriteRegister(NextPCReg, kernel->machine->ReadRegister(PCReg) + 4);
}

void setReturnCodeAndIncrementPC(int returnCode) {
  kernel->machine->WriteRegister(2, returnCode);
  incrementPC();
}
