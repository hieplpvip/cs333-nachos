// pcb.cc

#include "pcb.h"
#include "synch.h"
#include "thread.h"

PCB::PCB(int pid, int _parentId) {
  filename = NULL;
  joinSem = new Semaphore("joinSem", 0);
  exitSem = new Semaphore("exitSem", 0);
  thread = NULL;
  processID = pid;
  parentID = _parentId;
  exitCode = 0;
}

PCB::~PCB() {
  // DO NOT delete thread here
  // It is the job of the scheduler (see Scheduler::CheckToBeDestroyed)

  delete[] filename;
  delete joinSem;
  delete exitSem;
}

void startProcess(void* arg) {
  AddrSpace* space = (AddrSpace*)arg;
  space->Execute();
  ASSERTNOTREACHED();
}

int PCB::Execute(int argc, char** argv) {
  if (argc <= 0) {
    return -1;
  }

  ASSERT(filename == NULL);
  filename = new char[strlen(argv[0]) + 1];
  strcpy(filename, argv[0]);

  ASSERT(thread == NULL);
  thread = new Thread(filename);
  thread->pcb = this;

  AddrSpace* space = new AddrSpace();
  if (!space->Load(filename, argc, argv)) {
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
  joinSem->P();
}

void PCB::JoinRelease() {
  joinSem->V();
}

void PCB::ExitWait() {
  exitSem->P();
}

void PCB::ExitRelease() {
  exitSem->V();
}

void PCB::SetExitCode(int exitCode) {
  this->exitCode = exitCode;
}

int PCB::GetExitCode() const {
  return exitCode;
}

void PCB::setFileName(const char* name) {
  ASSERT(filename == NULL);
  filename = new char[strlen(name) + 1];
  strcpy(filename, name);
}

const char* PCB::GetFileName() const {
  return filename;
}
