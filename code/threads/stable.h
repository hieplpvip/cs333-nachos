// stable.h

#ifndef STABLE_H
#define STABLE_H

#include "bitmap.h"
#include "sem.h"

class STable {
private:
  int size;
  Bitmap* bm;  // Manage the free slot
  Sem** semTab;

  int FindSlotByName(const char* name) const;

public:
  STable(int size);
  ~STable();

  // Create a semaphore with name and initial value
  // Return 0 if success, -1 if error
  int Create(const char* name, int init);

  // Wait for the semaphore
  // Return 0 if success, -1 if error
  int Wait(const char* name);

  // Signal the semaphore
  // Return 0 if success, -1 if error
  int Signal(const char* name);
};

#endif  // STABLE_H
