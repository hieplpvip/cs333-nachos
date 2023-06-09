// ptable.h

#ifndef PTABLE_H
#define PTABLE_H

#include "bitmap.h"
#include "pcb.h"

class Semaphore;

class ProcessTable {
private:
  Bitmap* bitmap;
  Semaphore* bitmapMutex;
  PCB** table;

  // Remove the PCB entry of the process with PID = pid
  void Remove(int pid);

public:
  ProcessTable();
  ~ProcessTable();

  // Check if the PID is valid
  bool IsValidPID(int pid) const;

  // Get the PCB of the process with PID = pid
  PCB* GetPCB(int pid) const;

  // Get the PCB of the current process
  PCB* GetCurrentPCB() const;

  // Fork and exec a new process
  int ExecV(int argc, char const* const* argv);

  // Process the syscall SC_Join
  int Join(int childPid);

  // Process the syscall SC_Exit
  void Exit(int exitCode);
};

#endif  // PTABLE_H
