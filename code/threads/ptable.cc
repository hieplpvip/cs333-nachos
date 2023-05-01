// ptable.cc

#include "ptable.h"
#include "synch.h"

ProcessTable::ProcessTable() {
  bitmap = new Bitmap(NumProcesses);
  bitmapMutex = new Semaphore("bitmapMutex", 1);
  table = new PCB*[NumProcesses];
  for (int i = 0; i < NumProcesses; i++) {
    table[i] = NULL;
  }

  // Reserve pid 0 for the init process (i.e. current thread)
  bitmapMutex->P();
  bitmap->Mark(0);
  table[0] = new PCB(0, -1);
  table[0]->thread = kernel->currentThread;
  table[0]->setFileName(kernel->currentThread->getName());
  kernel->currentThread->pcb = table[0];
  bitmapMutex->V();
}

ProcessTable::~ProcessTable() {
  delete bitmap;
  delete bitmapMutex;
  for (int i = 0; i < NumProcesses; i++) {
    if (table[i]) {
      delete table[i];
    }
  }
  delete[] table;
}

void ProcessTable::Remove(int pid) {
  ASSERT(pid >= 0 && pid < NumProcesses);
  ASSERT(table[pid] != NULL);
  delete table[pid];
  table[pid] = NULL;
  bitmap->Clear(pid);
}

bool ProcessTable::IsValidPID(int pid) const {
  return (pid >= 0 && pid < NumProcesses && table[pid] != NULL);
}

PCB* ProcessTable::GetPCB(int pid) const {
  if (!IsValidPID(pid)) {
    return NULL;
  }
  return table[pid];
}

int ProcessTable::ExecV(int argc, char** argv) {
  if (argc <= 0) {
    return -1;
  }

  bitmapMutex->P();

  int pid = bitmap->FindAndSet();
  if (pid == -1) {
    // No free slot
    return -1;
  }

  table[pid] = new PCB(pid, kernel->currentThread->getProcessID());
  if (table[pid]->Execute(argc, argv) == -1) {
    // Failed to execute new process
    Remove(pid);
    pid = -1;
  }

  bitmapMutex->V();

  return pid;
}

int ProcessTable::Join(int childPid) {
  int pid = kernel->currentThread->getProcessID();
  if (!IsValidPID(childPid) || table[childPid]->GetParentID() != pid) {
    // Invalid child pid or not child of current process
    return -1;
  }

  // Wait for child process to exit
  table[childPid]->JoinWait();

  // Handle exit code of child process
  int exitCode = table[childPid]->GetExitCode();

  // Release child process so it can finally exit
  table[childPid]->ExitRelease();

  return exitCode;
}

void ProcessTable::Exit(int exitCode) {
  int pid = kernel->currentThread->getProcessID();
  if (pid == 0) {
    // Main process
    kernel->interrupt->Halt();
    ASSERTNOTREACHED();
  }

  table[pid]->SetExitCode(exitCode);

  // Release parent process if it is waiting for this process
  table[pid]->JoinRelease();

  // Wait for parent process to handle exit code
  table[pid]->ExitWait();

  Remove(pid);
  kernel->currentThread->Finish();
}
