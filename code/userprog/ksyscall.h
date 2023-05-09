/**************************************************************
 *
 * userprog/ksyscall.h
 *
 * Kernel interface for systemcalls
 *
 * by Marcus Voelp  (c) Universitaet Karlsruhe
 *
 **************************************************************/

#ifndef __USERPROG_KSYSCALL_H__
#define __USERPROG_KSYSCALL_H__

#include "kernel.h"
#include "main.h"
#include "synchconsole.h"

void SysHalt() {
  kernel->interrupt->Halt();
}

int SysAdd(int op1, int op2) {
  DEBUG(dbgSys, "Add " << op1 << " + " << op2);

  return op1 + op2;
}

int SysPrintString(const char* s) {
  DEBUG(dbgSys, "Print string");

  while (*s != '\0') {
    kernel->synchConsoleOut->PutChar(*s);
    s++;
  }
  return 0;
}

int SysCreate(const char* filename) {
  DEBUG(dbgSys, "Create file " << filename);

  if (!kernel->processTable->GetCurrentPCB()->GetFileTable()->Create(filename)) {
    DEBUG(dbgSys, "Error creating file " << filename);
    return -1;
  }
  return 0;
}

int SysOpen(const char* filename, int mode) {
  DEBUG(dbgSys, "Open file " << filename);

  int result = kernel->processTable->GetCurrentPCB()->GetFileTable()->Open(filename, mode);
  if (result == -1) {
    DEBUG(dbgSys, "Error opening file " << filename);
    return -1;
  }
  return result;
}

int SysClose(int fileId) {
  DEBUG(dbgSys, "Close file id " << fileId);

  if (!kernel->processTable->GetCurrentPCB()->GetFileTable()->Close(fileId)) {
    DEBUG(dbgSys, "Error closing file id " << fileId);
    return -1;
  }
  return 0;
}

int SysRead(char* buffer, int count, int fileId) {
  DEBUG(dbgSys, "Read file id " << fileId);

  if (count <= 0) {
    DEBUG(dbgSys, "count must be non-negative");
    return -1;
  }

  return kernel->processTable->GetCurrentPCB()->GetFileTable()->Read(fileId, buffer, count);
}

int SysWrite(const char* buffer, int count, int fileId) {
  DEBUG(dbgSys, "Write file id " << fileId);

  if (count <= 0) {
    DEBUG(dbgSys, "count must be non-negative");
    return -1;
  }

  return kernel->processTable->GetCurrentPCB()->GetFileTable()->Write(fileId, buffer, count);
}

int SysSeek(int pos, int fileId) {
  DEBUG(dbgSys, "Seek file id " << fileId);

  int result = kernel->processTable->GetCurrentPCB()->GetFileTable()->Seek(fileId, pos);
  if (result == -1) {
    DEBUG(dbgSys, "Error seeking file id " << fileId);
    return -1;
  }
  return result;
}

int SysRemove(const char* filename) {
  DEBUG(dbgSys, "Remove file " << filename);

  if (!kernel->processTable->GetCurrentPCB()->GetFileTable()->Remove(filename)) {
    DEBUG(dbgSys, "Error removing file " << filename);
    return -1;
  }
  return 0;
}

int SysOpenSocket() {
  DEBUG(dbgSys, "Create socket");

  int result = kernel->processTable->GetCurrentPCB()->GetFileTable()->CreateTCPSocket();
  if (result == -1) {
    DEBUG(dbgSys, "Error creating socket");
    return -1;
  }
  return result;
}

int SysConnect(int socketId, const char* ip, int port) {
  DEBUG(dbgSys, "Connect socket id " << socketId << " to " << ip << ":" << port);

  if (kernel->processTable->GetCurrentPCB()->GetFileTable()->ConnectTCPSocket(socketId, ip, port) == -1) {
    DEBUG(dbgSys, "Error connecting socket id " << socketId << " to " << ip << ":" << port);
    return -1;
  }
  return 0;
}

int SysSend(int socketId, const char* buffer, int count) {
  DEBUG(dbgSys, "Send data");

  int result = kernel->processTable->GetCurrentPCB()->GetFileTable()->SendData(socketId, buffer, count);
  if (result == -1) {
    DEBUG(dbgSys, "Error sending data");
  } else if (result == 0) {
    DEBUG(dbgSys, "Connection closed");
  } else {
    DEBUG(dbgSys, "Sent " << result << " bytes of data");
  }
  return result;
}

int SysReceive(int socketId, char* buffer, int count) {
  DEBUG(dbgSys, "Receive data");

  int result = kernel->processTable->GetCurrentPCB()->GetFileTable()->ReceiveData(socketId, buffer, count);
  if (result == -1) {
    DEBUG(dbgSys, "Error receiving data");
  } else if (result == 0) {
    DEBUG(dbgSys, "Connection closed");
  } else {
    DEBUG(dbgSys, "Received " << result << " bytes of data");
  }
  return result;
}

int SysCloseSocket(int socketId) {
  DEBUG(dbgSys, "Close socket");

  if (!kernel->processTable->GetCurrentPCB()->GetFileTable()->CloseTCPSocket(socketId)) {
    DEBUG(dbgSys, "Error closing socket id " << socketId);
    return -1;
  }
  return 0;
}

int SysExecV(int argc, char const* const* argv) {
  DEBUG(dbgSys, "ExecV " << argc << ' ' << argv[0]);

  return kernel->processTable->ExecV(argc, argv);
}

int SysJoin(int pid) {
  DEBUG(dbgSys, "Join " << pid);

  return kernel->processTable->Join(pid);
}

void SysExit(int code) {
  DEBUG(dbgSys, "Exit with code " << code);

  kernel->processTable->Exit(code);
  ASSERTNOTREACHED();
}

int SysCreateSemaphore(const char* name, int initialValue) {
  DEBUG(dbgSys, "Create semaphore " << name << ", initial value " << initialValue);

  return kernel->semaphoreTable->Create(name, initialValue);
}

int SysWait(const char* name) {
  DEBUG(dbgSys, "Wait " << name);

  return kernel->semaphoreTable->Wait(name);
}

int SysSignal(const char* name) {
  DEBUG(dbgSys, "Signal " << name);

  return kernel->semaphoreTable->Signal(name);
}

#endif /* ! __USERPROG_KSYSCALL_H__ */
