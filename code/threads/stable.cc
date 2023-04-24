// stable.cc

#include "stable.h"
#include "string.h"

STable::STable(int _size) {
  size = _size;
  bm = new Bitmap(size);
  semTab = new Sem*[size];
  for (int i = 0; i < size; i++) {
    semTab[i] = NULL;
  }
}

STable::~STable() {
  delete bm;
  for (int i = 0; i < size; i++) {
    if (semTab[i]) {
      delete semTab[i];
    }
  }
  delete[] semTab;
}

int STable::FindSlotByName(const char* name) const {
  for (int i = 0; i < size; i++) {
    if (bm->Test(i) && strcmp(name, semTab[i]->getName()) == 0) {
      return i;
    }
  }
  return -1;
}

int STable::Create(const char* name, int init) {
  int slot = FindSlotByName(name);
  if (slot != -1) {
    // A semaphore with this name already exists
    return -1;
  }

  slot = bm->FindAndSet();
  if (slot == -1) {
    // No free slot
    return -1;
  }

  semTab[slot] = new Sem(name, init);
  return 0;
}

int STable::Wait(const char* name) {
  int slot = FindSlotByName(name);
  if (slot == -1) {
    // No semaphore with this name
    return -1;
  }

  semTab[slot]->wait();
  return 0;
}

int STable::Signal(const char* name) {
  int slot = FindSlotByName(name);
  if (slot == -1) {
    // No semaphore with this name
    return -1;
  }

  semTab[slot]->signal();
  return 0;
}
