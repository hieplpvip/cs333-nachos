// ptable.h

#ifndef PTABLE_H
#define PTABLE_H

#include "bitmap.h"
#include "pcb.h"

class Semaphore;

class PTable {
private:
  int size;
  Bitmap* bm;        // mark the locations that have been used in pcb
  Semaphore* bmsem;  // used to prevent the case of loading 2 processes at the same time
  PCB** pcb;

  // Remove the PCB entry of the process with PID = pid
  void Remove(int pid);

public:
  PTable(int size);
  ~PTable();

  // Process the syscall SC_Exec
  int ExecUpdate(const char*);

  // Process the syscall SC_Exit
  int ExitUpdate(int exitCode);

  // Process the syscall SC_Join
  int JoinUpdate(int childPid);

  // Check a process exist or not
  bool IsExist(int pid) const;

  // Return the process's file name
  const char* GetFileName(int pid) const;
};

#endif  // PTABLE_H
