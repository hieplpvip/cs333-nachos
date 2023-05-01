// exception.cc
//	Entry point into the Nachos kernel from user programs.
//	There are two kinds of things that can cause control to
//	transfer back to here from user code:
//
//	syscall -- The user code explicitly requests to call a procedure
//	in the Nachos kernel.  Right now, the only function we support is
//	"Halt".
//
//	exceptions -- The user code does something that the CPU can't handle.
//	For instance, accessing memory that doesn't exist, arithmetic errors,
//	etc.
//
//	Interrupts (which can also cause control to transfer from user
//	code into the Nachos kernel) are handled elsewhere.
//
// For now, this only handles the Halt() system call.
// Everything else core dumps.
//
// Copyright (c) 1992-1996 The Regents of the University of California.
// All rights reserved.  See copyright.h for copyright notice and limitation
// of liability and disclaimer of warranty provisions.

#include "copyright.h"
#include "ksyscall.h"
#include "main.h"
#include "syscall.h"
#include "utils.h"

//----------------------------------------------------------------------
// ExceptionHandler
// 	Entry point into the Nachos kernel.  Called when a user program
//	is executing, and either does a syscall, or generates an addressing
//	or arithmetic exception.
//
// 	For system calls, the following is the calling convention:
//
// 	system call code -- r2
//		arg1 -- r4
//		arg2 -- r5
//		arg3 -- r6
//		arg4 -- r7
//
//	The result of the system call, if any, must be put back into r2.
//
// If you are handling a system call, don't forget to increment the pc
// before returning. (Or else you'll loop making the same system call forever!)
//
//	"which" is the kind of exception.  The list of possible exceptions
//	is in machine.h.
//----------------------------------------------------------------------

#define MAX_FILE_NAME_LENGTH 32
#define MAX_IP_ADDRESS_LENGTH 15

void handle_SC_Halt() {
  DEBUG(dbgSys, "Shutdown, initiated by user program.\n");
  SysHalt();
  ASSERTNOTREACHED();
}

void handle_SC_Add() {
  DEBUG(dbgSys, "Handle SC_Add");

  int op1 = kernel->machine->ReadRegister(4);
  int op2 = kernel->machine->ReadRegister(5);
  int result = SysAdd(op1, op2);
  return setReturnCodeAndAdvancePC(result);
}

void handle_SC_PrintString() {
  DEBUG(dbgSys, "Handle SC_PrintString");

  int virtAddr = kernel->machine->ReadRegister(4);
  int result;

  char* s = StringUser2System(virtAddr);
  result = SysPrintString(s);
  delete[] s;

  return setReturnCodeAndAdvancePC(result);
}

void handle_SC_Create() {
  DEBUG(dbgSys, "Handle SC_Create");

  int virtAddr = kernel->machine->ReadRegister(4);
  int result;

  char* filename = new char[MAX_FILE_NAME_LENGTH + 1];
  StringUser2System(virtAddr, MAX_FILE_NAME_LENGTH, filename);
  result = SysCreate(filename);
  delete[] filename;

  return setReturnCodeAndAdvancePC(result);
}

void handle_SC_Open() {
  DEBUG(dbgSys, "Handle SC_Open");

  int virtAddr = kernel->machine->ReadRegister(4);
  int mode = kernel->machine->ReadRegister(5);
  int result;

  char* filename = new char[MAX_FILE_NAME_LENGTH + 1];
  StringUser2System(virtAddr, MAX_FILE_NAME_LENGTH, filename);
  result = SysOpen(filename, mode);
  delete[] filename;

  return setReturnCodeAndAdvancePC(result);
}

void handle_SC_Close() {
  DEBUG(dbgSys, "Handle SC_Close");

  int fileId = kernel->machine->ReadRegister(4);
  int result = SysClose(fileId);
  return setReturnCodeAndAdvancePC(result);
}

void handle_SC_Read() {
  DEBUG(dbgSys, "Handle SC_Read");

  int virtAddr = kernel->machine->ReadRegister(4);
  int count = kernel->machine->ReadRegister(5);
  int fileId = kernel->machine->ReadRegister(6);
  int result;

  char* buffer = new char[count];
  result = SysRead(buffer, count, fileId);
  if (result > 0) {
    RawSystem2User(virtAddr, result, buffer);
  }
  delete[] buffer;

  return setReturnCodeAndAdvancePC(result);
}

void handle_SC_Write() {
  DEBUG(dbgSys, "Handle SC_Write");

  int virtAddr = kernel->machine->ReadRegister(4);
  int count = kernel->machine->ReadRegister(5);
  int fileId = kernel->machine->ReadRegister(6);
  int result;

  char* buffer = new char[count];
  RawUser2System(virtAddr, count, buffer);
  result = SysWrite(buffer, count, fileId);
  delete[] buffer;

  return setReturnCodeAndAdvancePC(result);
}

void handle_SC_Seek() {
  DEBUG(dbgSys, "Handle SC_Seek");

  int pos = kernel->machine->ReadRegister(4);
  int fileId = kernel->machine->ReadRegister(5);
  int result = SysSeek(pos, fileId);
  return setReturnCodeAndAdvancePC(result);
}

void handle_SC_Remove() {
  DEBUG(dbgSys, "Handle SC_Remove");

  int virtAddr = kernel->machine->ReadRegister(4);
  int result;

  char* filename = new char[MAX_FILE_NAME_LENGTH + 1];
  StringUser2System(virtAddr, MAX_FILE_NAME_LENGTH, filename);
  result = SysRemove(filename);
  delete[] filename;

  return setReturnCodeAndAdvancePC(result);
}

void handle_SC_OpenSocket() {
  DEBUG(dbgSys, "Handle SC_OpenSocket");

  int result = SysOpenSocket();
  return setReturnCodeAndAdvancePC(result);
}

void handle_SC_Connect() {
  DEBUG(dbgSys, "Handle SC_Connect");

  int socketId = kernel->machine->ReadRegister(4);
  int virtAddr = kernel->machine->ReadRegister(5);
  int port = kernel->machine->ReadRegister(6);
  int result;

  char* ip = new char[MAX_IP_ADDRESS_LENGTH + 1];
  StringUser2System(virtAddr, MAX_IP_ADDRESS_LENGTH, ip);
  result = SysConnect(socketId, ip, port);
  delete[] ip;

  return setReturnCodeAndAdvancePC(result);
}

void handle_SC_Send() {
  DEBUG(dbgSys, "Handle SC_Send");

  int socketId = kernel->machine->ReadRegister(4);
  int virtAddr = kernel->machine->ReadRegister(5);
  int len = kernel->machine->ReadRegister(6);
  int result;

  if (len <= 0) {
    DEBUG(dbgAddr, "Invalid length");
    return setReturnCodeAndAdvancePC(-1);
  }

  char* buffer = new char[len];
  RawUser2System(virtAddr, len, buffer);
  result = SysSend(socketId, buffer, len);
  delete[] buffer;

  return setReturnCodeAndAdvancePC(result);
}

void handle_SC_Receive() {
  DEBUG(dbgSys, "Handle SC_Receive");

  int socketID = kernel->machine->ReadRegister(4);
  int virtAddr = kernel->machine->ReadRegister(5);
  int len = kernel->machine->ReadRegister(6);
  int result;

  char* buffer = new char[len];
  result = SysReceive(socketID, buffer, len);
  if (result > 0) {
    RawSystem2User(virtAddr, result, buffer);
  }
  delete[] buffer;

  return setReturnCodeAndAdvancePC(result);
}

void handle_SC_CloseSocket() {
  DEBUG(dbgSys, "Handle SC_CloseSocket");

  int socketID = kernel->machine->ReadRegister(4);
  int result = SysCloseSocket(socketID);
  return setReturnCodeAndAdvancePC(result);
}

void handle_SC_Exec() {
  DEBUG(dbgSys, "Handle SC_Exec");

  int virtAddr = kernel->machine->ReadRegister(4);
  int result;

  // Copy filename from user space to kernel space and put it in argv[0],
  // then call SysExecV
  int argc = 1;
  char* argv[1];
  argv[0] = StringUser2System(virtAddr);
  result = SysExecV(argc, argv);
  delete[] argv[0];

  return setReturnCodeAndAdvancePC(result);
}

void handle_SC_ExecV() {
  DEBUG(dbgSys, "Handle SC_ExecV");

  int argc = kernel->machine->ReadRegister(4);
  int argvUser = kernel->machine->ReadRegister(5);
  int result;

  // Copy argv from user space to kernel space
  char** argv = new char*[argc];
  for (int i = 0; i < argc; i++) {
    int virtAddr;
    kernel->machine->ReadMem(argvUser + i * 4, 4, &virtAddr);
    argv[i] = StringUser2System(virtAddr);
  }

  // Execute
  result = SysExecV(argc, argv);

  // Free argv
  for (int i = 0; i < argc; i++) {
    delete[] argv[i];
  }
  delete[] argv;

  return setReturnCodeAndAdvancePC(result);
}

void handle_SC_Join() {
  DEBUG(dbgSys, "Handle SC_Join");

  int pid = kernel->machine->ReadRegister(4);
  int result = SysJoin(pid);
  return setReturnCodeAndAdvancePC(result);
}

void handle_SC_Exit() {
  DEBUG(dbgSys, "Handle SC_Exit");

  int code = kernel->machine->ReadRegister(4);
  SysExit(code);
  ASSERTNOTREACHED();
}

void handle_SC_CreateSemaphore() {
  DEBUG(dbgSys, "Handle SC_CreateSemaphore");

  int virtAddr = kernel->machine->ReadRegister(4);
  int initialValue = kernel->machine->ReadRegister(5);
  int result;

  char* name = StringUser2System(virtAddr);
  result = SysCreateSemaphore(name, initialValue);
  delete[] name;

  return setReturnCodeAndAdvancePC(result);
}

void handle_SC_Wait() {
  DEBUG(dbgSys, "Handle SC_Wait");

  int virtAddr = kernel->machine->ReadRegister(4);
  int result;

  char* name = StringUser2System(virtAddr);
  result = SysWait(name);
  delete[] name;

  return setReturnCodeAndAdvancePC(result);
}

void handle_SC_Signal() {
  DEBUG(dbgSys, "Handle SC_Signal");

  int virtAddr = kernel->machine->ReadRegister(4);
  int result;

  char* name = StringUser2System(virtAddr);
  result = SysSignal(name);
  delete[] name;

  return setReturnCodeAndAdvancePC(result);
}

void ExceptionHandler(ExceptionType which) {
  int type = kernel->machine->ReadRegister(2);

  DEBUG(dbgSys, "Received Exception " << which << " type: " << type << "\n");

  switch (which) {
    case NoException: {
      return;
    }

    case SyscallException:
      switch (type) {
        case SC_Halt:
          return handle_SC_Halt();
        case SC_Add:
          return handle_SC_Add();
        case SC_PrintString:
          return handle_SC_PrintString();
        case SC_Create:
          return handle_SC_Create();
        case SC_Open:
          return handle_SC_Open();
        case SC_Close:
          return handle_SC_Close();
        case SC_Read:
          return handle_SC_Read();
        case SC_Write:
          return handle_SC_Write();
        case SC_Seek:
          return handle_SC_Seek();
        case SC_Remove:
          return handle_SC_Remove();
        case SC_OpenSocket:
          return handle_SC_OpenSocket();
        case SC_Connect:
          return handle_SC_Connect();
        case SC_Send:
          return handle_SC_Send();
        case SC_Receive:
          return handle_SC_Receive();
        case SC_CloseSocket:
          return handle_SC_CloseSocket();
        case SC_Exec:
          return handle_SC_Exec();
        case SC_ExecV:
          return handle_SC_ExecV();
        case SC_Join:
          return handle_SC_Join();
        case SC_Exit:
          return handle_SC_Exit();
        case SC_CreateSemaphore:
          return handle_SC_CreateSemaphore();
        case SC_Wait:
          return handle_SC_Wait();
        case SC_Signal:
          return handle_SC_Signal();

        default: {
          cerr << "Unexpected system call " << type << "\n";
          break;
        }
      }
      break;

    default: {
      cerr << "Unexpected user mode exception " << (int)which << "\n";
      break;
    }
  }
  ASSERTNOTREACHED();
}
