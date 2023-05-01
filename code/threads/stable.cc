// stable.cc

#include "stable.h"
#include "synch.h"
#include "string.h"

SemaphoreTable::SemaphoreTable(int _size) {
  size = _size;
  bitmap = new Bitmap(size);
  table = new Semaphore*[size];
  for (int i = 0; i < size; i++) {
    table[i] = NULL;
  }
}

SemaphoreTable::~SemaphoreTable() {
  delete bitmap;
  for (int i = 0; i < size; i++) {
    if (table[i]) {
      delete[] table[i]->getName();
      delete table[i];
    }
  }
  delete[] table;
}

int SemaphoreTable::FindSlotByName(const char* name) const {
  for (int i = 0; i < size; i++) {
    if (bitmap->Test(i) && strcmp(name, table[i]->getName()) == 0) {
      return i;
    }
  }
  return -1;
}

int SemaphoreTable::Create(const char* _name, int initialValue) {
  ASSERT(_name != NULL);

  int slot = FindSlotByName(_name);
  if (slot != -1) {
    // A semaphore with this name already exists
    return -1;
  }

  slot = bitmap->FindAndSet();
  if (slot == -1) {
    // No free slot
    return -1;
  }

  char* name = new char[strlen(_name) + 1];
  strcpy(name, _name);
  table[slot] = new Semaphore(name, initialValue);
  return 0;
}

int SemaphoreTable::Wait(const char* name) {
  int slot = FindSlotByName(name);
  if (slot == -1) {
    // No semaphore with this name
    return -1;
  }

  table[slot]->P();
  return 0;
}

int SemaphoreTable::Signal(const char* name) {
  int slot = FindSlotByName(name);
  if (slot == -1) {
    // No semaphore with this name
    return -1;
  }

  table[slot]->V();
  return 0;
}
