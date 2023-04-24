// pcb.cc

#include "pcb.h"
#include "synch.h"
#include "thread.h"

PCB::PCB(const char* _filename, int pid, int _parentId) {
  ASSERT(_filename != NULL);
  filename = new char[strlen(_filename) + 1];
  strcpy(filename, _filename);

  joinsem = new Semaphore("joinsem", 0);
  exitsem = new Semaphore("exitsem", 0);
  thread = NULL;
  processID = pid;
  parentID = _parentId;
  exitCode = 0;
}

PCB::~PCB() {
  // DO NOT delete thread here
  // It is the job of the scheduler (see Scheduler::CheckToBeDestroyed)

  delete[] filename;
  delete joinsem;
  delete exitsem;
}

void startProcess(void* arg) {
  AddrSpace* space = (AddrSpace*)arg;
  space->Execute();
  ASSERTNOTREACHED();
}

int PCB::Exec() {
  ASSERT(thread == NULL);
  thread = new Thread(filename);
  thread->pcb = this;

  AddrSpace* space = new AddrSpace();
  // FIXME: copy filename to argv[0]
  if (!space->Load(filename, 0, NULL)) {
    return -1;
  }

  // Fork and call space->execute() in newly created thread
  thread->Fork(startProcess, space);

  return 0;
}

int PCB::GetProcessID() const {
  return processID;
}

int PCB::GetParentID() const {
  return parentID;
}

void PCB::JoinWait() {
  joinsem->P();
}

void PCB::JoinRelease() {
  joinsem->V();
}

void PCB::ExitWait() {
  exitsem->P();
}

void PCB::ExitRelease() {
  exitsem->V();
}

void PCB::SetExitCode(int exitCode) {
  this->exitCode = exitCode;
}

int PCB::GetExitCode() const {
  return exitCode;
}

const char* PCB::GetFileName() const {
  return filename;
}
