// ptable.cc

#include "ptable.h"
#include "synch.h"

PTable::PTable(int _size) {
  size = _size;
  bm = new Bitmap(size);
  bmsem = new Semaphore("bmsem", 1);
  pcb = new PCB*[size];
  for (int i = 0; i < size; i++) {
    pcb[i] = NULL;
  }

  // Reserve pid 0 for the init process (i.e. current thread)
  bmsem->P();
  bm->Mark(0);
  pcb[0] = new PCB(0, -1);
  pcb[0]->thread = kernel->currentThread;
  pcb[0]->setFileName(kernel->currentThread->getName());
  kernel->currentThread->pcb = pcb[0];
  bmsem->V();
}

PTable::~PTable() {
  delete bm;
  delete bmsem;
  for (int i = 0; i < size; i++) {
    if (pcb[i]) {
      delete pcb[i];
    }
  }
  delete[] pcb;
}

void PTable::Remove(int pid) {
  ASSERT(pid >= 0 && pid < size);
  ASSERT(pcb[pid] != NULL);
  delete pcb[pid];
  pcb[pid] = NULL;
  bm->Clear(pid);
}

int PTable::ExecUpdate(int argc, char** argv) {
  if (argc <= 0) {
    return -1;
  }

  bmsem->P();

  int pid = bm->FindAndSet();
  if (pid == -1) {
    // No free slot
    return -1;
  }

  pcb[pid] = new PCB(pid, kernel->currentThread->getProcessID());
  if (pcb[pid]->Exec(argc, argv) == -1) {
    // Failed to start new process
    Remove(pid);
    pid = -1;
  }

  bmsem->V();

  return pid;
}

void PTable::ExitUpdate(int exitCode) {
  int pid = kernel->currentThread->getProcessID();
  if (pid == 0) {
    // Main process
    kernel->interrupt->Halt();
    ASSERTNOTREACHED();
  }

  pcb[pid]->SetExitCode(exitCode);

  // Release parent process if it is waiting for this process
  pcb[pid]->JoinRelease();

  // Wait for parent process to handle exit code
  pcb[pid]->ExitWait();

  Remove(pid);
  kernel->currentThread->Finish();
}

int PTable::JoinUpdate(int childPid) {
  int pid = kernel->currentThread->getProcessID();
  if (!IsExist(childPid) || pcb[childPid]->GetParentID() != pid) {
    // Invalid child pid or not child of current process
    return -1;
  }

  // Wait for child process to exit
  pcb[childPid]->JoinWait();

  // Handle exit code of child process
  int exitCode = pcb[childPid]->GetExitCode();

  // Release child process so it can finally exit
  pcb[childPid]->ExitRelease();

  return exitCode;
}

bool PTable::IsExist(int pid) const {
  return (pid >= 0 && pid < size && pcb[pid] != NULL);
}

const char* PTable::GetFileName(int pid) const {
  ASSERT(pid >= 0 && pid < size);
  return pcb[pid]->GetFileName();
}
