// stable.h

#ifndef STABLE_H
#define STABLE_H

#include "bitmap.h"

class Semaphore;

class SemaphoreTable {
private:
  int size;
  Bitmap* bitmap;  // Manage the free slot
  Semaphore** table;

  int FindSlotByName(const char* name) const;

public:
  SemaphoreTable(int size);
  ~SemaphoreTable();

  // Create a semaphore with name and initial value
  // Return 0 if success, -1 if error
  int Create(const char* name, int initialValue);

  // Wait for the semaphore
  // Return 0 if success, -1 if error
  int Wait(const char* name);

  // Signal the semaphore
  // Return 0 if success, -1 if error
  int Signal(const char* name);
};

#endif  // STABLE_H
