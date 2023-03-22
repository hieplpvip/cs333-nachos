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

void SysHalt() {
  kernel->interrupt->Halt();
}

int SysAdd(int op1, int op2) {
  DEBUG(dbgSys, "Add " << op1 << " + " << op2 << "\n");

  return op1 + op2;
}

int SysCreate(char* filename) {
  DEBUG(dbgSys, "Create file " << filename);

  if (!kernel->fileSystem->Create(filename)) {
    DEBUG(dbgSys, "Error creating file " << filename);
    return -1;
  }
  return 0;
}

int SysOpen(char* filename, int mode) {
  DEBUG(dbgSys, "Open file " << filename);

  int result = kernel->fileSystem->Open(filename, mode);
  if (result == -1) {
    DEBUG(dbgSys, "Error opening file " << filename);
    return -1;
  }
  return result;
}

int SysClose(int fileId) {
  DEBUG(dbgSys, "Close file id " << fileId);

  if (!kernel->fileSystem->Close(fileId)) {
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

  return kernel->fileSystem->Read(fileId, buffer, count);
}

int SysWrite(char* buffer, int count, int fileId) {
  DEBUG(dbgSys, "Write file id " << fileId);

  if (count <= 0) {
    DEBUG(dbgSys, "count must be non-negative");
    return -1;
  }

  return kernel->fileSystem->Write(fileId, buffer, count);
}

int SysSeek(int pos, int fileId) {
  DEBUG(dbgSys, "Seek file id " << fileId);

  int result = kernel->fileSystem->Seek(pos, fileId);
  if (result == -1) {
    DEBUG(dbgSys, "Error seeking file id " << fileId);
    return -1;
  }
  return result;
}

int SysRemove(char* filename) {
  DEBUG(dbgSys, "Remove file " << filename);

  if (!kernel->fileSystem->Remove(filename)) {
    DEBUG(dbgSys, "Error removing file " << filename);
    return -1;
  }
  return 0;
}

int SysOpenSocket() {
  DEBUG(dbgSys, "Create socket");

  int result = kernel->fileSystem->CreateTCPSocket();
  if (result == -1) {
    DEBUG(dbgSys, "Error creating socket");
    return -1;
  }
  return result;
}

int SysConnect(int socketId, char* ip, int port) {
  DEBUG(dbgSys, "Connect socket id " << socketId << " to " << ip << ":" << port);

  if (kernel->fileSystem->ConnectTCPSocket(socketId, ip, port) == -1) {
    DEBUG(dbgSys, "Error connecting socket id " << socketId << " to " << ip << ":" << port);
    return -1;
  }
  return 0;
}

int SysSend(int socketId, char* buffer, int count) {
  DEBUG(dbgSys, "Send data");

  int result = kernel->fileSystem->SendData(socketId, buffer, count);
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

  int result = kernel->fileSystem->ReceiveData(socketId, buffer, count);
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

  if (!kernel->fileSystem->CloseTCPSocket(socketId)) {
    DEBUG(dbgSys, "Error closing socket id " << socketId);
    return -1;
  }
  return 0;
}

#endif /* ! __USERPROG_KSYSCALL_H__ */
