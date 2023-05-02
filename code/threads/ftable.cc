// ftable.cc

#include "ftable.h"
#include "main.h"

#define VALID_SLOT(slot) (slot >= 0 && slot < NumOpenFilesPerProcess && table[slot] != -1)

FileTable::FileTable() {
  for (int i = 0; i < NumOpenFilesPerProcess; i++) {
    table[i] = -1;
  }

  // Open stdin and stdout by default
  table[0] = 0;
  table[1] = 1;
}

FileTable::~FileTable() {
  // Close all files
  for (int slot = 2; slot < NumOpenFilesPerProcess; slot++) {
    if (table[slot] != -1) {
      ASSERT(kernel->fileSystem->Close(table[slot]));
      table[slot] = -1;
    }
  }
}

int FileTable::findFreeSlot() const {
  for (int slot = 0; slot < NumOpenFilesPerProcess; slot++) {
    if (table[slot] == -1) {
      return slot;
    }
  }

  return -1;
}

bool FileTable::Create(char *name) {
  return kernel->fileSystem->Create(name);
}

int FileTable::Open(char *name, int mode) {
  int slot = findFreeSlot();
  if (slot == -1) {
    return -1;
  }

  int fd = kernel->fileSystem->Open(name, mode);
  if (fd < 0) {
    return -1;
  }

  table[slot] = fd;
  return slot;
}

bool FileTable::Close(int slot) {
  if (!VALID_SLOT(slot)) {
    return FALSE;
  }

  bool result = kernel->fileSystem->Close(table[slot]);
  table[slot] = -1;
  return result;
}

bool FileTable::Remove(char *name) {
  return kernel->fileSystem->Remove(name);
}

int FileTable::Read(int slot, char *buffer, int count) {
  if (!VALID_SLOT(slot)) {
    return -1;
  }

  return kernel->fileSystem->Read(table[slot], buffer, count);
}

int FileTable::Write(int slot, char *buffer, int count) {
  if (!VALID_SLOT(slot)) {
    return -1;
  }

  return kernel->fileSystem->Write(table[slot], buffer, count);
}

int FileTable::Seek(int slot, int pos) {
  if (!VALID_SLOT(slot)) {
    return -1;
  }

  return kernel->fileSystem->Seek(table[slot], pos);
}

int FileTable::CreateTCPSocket() {
  int slot = findFreeSlot();
  if (slot == -1) {
    return -1;
  }

  int fd = kernel->fileSystem->CreateTCPSocket();
  if (fd < 0) {
    return -1;
  }

  table[slot] = fd;
  return slot;
}

int FileTable::ConnectTCPSocket(int slot, char *ip, int port) {
  if (!VALID_SLOT(slot)) {
    return -1;
  }

  return kernel->fileSystem->ConnectTCPSocket(table[slot], ip, port);
}

int FileTable::SendData(int slot, char *buffer, int count) {
  if (!VALID_SLOT(slot)) {
    return -1;
  }

  return kernel->fileSystem->SendData(table[slot], buffer, count);
}

int FileTable::ReceiveData(int slot, char *buffer, int count) {
  if (!VALID_SLOT(slot)) {
    return -1;
  }

  return kernel->fileSystem->ReceiveData(table[slot], buffer, count);
}

bool FileTable::CloseTCPSocket(int slot) {
  if (!VALID_SLOT(slot)) {
    return FALSE;
  }

  bool result = kernel->fileSystem->CloseTCPSocket(table[slot]);
  table[slot] = -1;
  return result;
}
