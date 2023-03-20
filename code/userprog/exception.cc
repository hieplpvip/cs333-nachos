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
#include "main.h"
#include "syscall.h"
#include "ksyscall.h"
#include "util.c"
#include "synchconsole.h"
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

void SysOpenSocket() {
  DEBUG('a', "\n SC_OpenSocket calls ...");
  int SocketID;
  if ((SocketID = kernel->fileSystem->CreateTCPSocket()) == -1) {
    printf("\n Error Create Socket... ");
    kernel->machine->WriteRegister(2, -1);
    incrementPC();
    return;
  }
  printf("\n Create Socket Successful, SocketId: %d", SocketID);
  kernel->machine->WriteRegister(2, SocketID);
  incrementPC();
}

void SysConnect() {
  DEBUG('a', "\n SC_Connect calls ...");
  int SocketID, port, virtAddr;
  char* ip;

  DEBUG('a', "\n Reading SocketID");
  SocketID = kernel->machine->ReadRegister(4);

  DEBUG('a', "\n Reading virtual address of IP");
  virtAddr = kernel->machine->ReadRegister(5);
  DEBUG('a', "\n Reading IP.");
  ip = User2System(virtAddr, MAX_IP_ADDRESS_LENGTH);
  if (ip == NULL) {
    kernel->machine->WriteRegister(2, -1);
    delete ip;
    incrementPC();
    return;
  }
  DEBUG('a', "\n Finish reading IP.");

  DEBUG('a', "\n Reading Port.");
  port = kernel->machine->ReadRegister(6);

  if (kernel->fileSystem->ConnectTCPSocket(SocketID, ip, port) == -1) {
    printf("\n Failed to connect to SocketId: %d, IP: %s, Port: %d", SocketID, ip, port);
    kernel->machine->WriteRegister(2, -1);
    delete ip;
    incrementPC();
    return;
  }

  printf("\n Connect Socket Successful, SocketId: %d, IP: %s, Port: %d", SocketID, ip, port);
  kernel->machine->WriteRegister(2, 0);
  delete ip;
  incrementPC();
}

void SysSend() {
  int SocketID, len, virtAddr, retVal;
  char* buffer;
  DEBUG('a', "\n SC_Send calls ...");
  DEBUG('a', "\n Reading SocketID");
  SocketID = kernel->machine->ReadRegister(4);

  DEBUG('a', "\n Reading virtual address of buffer");
  virtAddr = kernel->machine->ReadRegister(5);
  DEBUG('a', "\n Reading buffer.");
  buffer = User2System(virtAddr, len);
  if (buffer == NULL) {
    DEBUG('a', "\n Not enough memory in system");
    kernel->machine->WriteRegister(2, -1);
    delete buffer;
    incrementPC();
    return;
  }
  DEBUG('a', "\n Finish reading buffer.");

  DEBUG('a', "\n Reading len.");
  len = kernel->machine->ReadRegister(6);

  retVal = kernel->fileSystem->SendData(SocketID, buffer, len);

  if (retVal == -1) {
    printf("\n Failed to send data. ");
  } else if (retVal == 0) {
    printf("\n Connection closed. ");
  } else {
    printf("\n Sent %d bytes of data successfully. ", retVal);
  }

  kernel->machine->WriteRegister(2, retVal);
  delete buffer;
  incrementPC();
}

void SysReceive() {
  int SocketID, len, virtAddr, retVal;
  char* buffer;
  DEBUG('a', "\n SC_Receive calls ...");
  DEBUG('a', "\n Reading SocketID");
  SocketID = kernel->machine->ReadRegister(4);

  DEBUG('a', "\n Reading virtual address of buffer");
  virtAddr = kernel->machine->ReadRegister(5);

  DEBUG('a', "\n Reading len.");
  len = kernel->machine->ReadRegister(6);

  buffer = new char[len];

  retVal = kernel->fileSystem->ReceiveData(SocketID, buffer, len);

  if (retVal == -1) {
    printf("\n Failed to send data. ");
  } else if (retVal == 0) {
    printf("\n Connection closed. ");
  } else {
    printf("\n Receive %d bytes of data successfully. ", retVal);
    retVal = System2User(virtAddr, retVal, buffer);
  }
  kernel->machine->WriteRegister(2, retVal);
  delete buffer;
  incrementPC();
}

void SysCloseSocket() {
  int SocketID, retVal;
  DEBUG('a', "\n SC_CloseSocket calls ...");
  DEBUG('a', "\n Reading SocketID");
  SocketID = kernel->machine->ReadRegister(4);

  retVal = kernel->fileSystem->CloseTCPSocket(SocketID);

  kernel->machine->WriteRegister(2, retVal);
  incrementPC();
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
        case SC_Halt: {
          DEBUG(dbgSys, "Shutdown, initiated by user program.\n");

          SysHalt();

          ASSERTNOTREACHED();
          break;
        }
        case SC_PrintString: {
          int virtAddr;
          int len;
          char* s;
          virtAddr = kernel->machine->ReadRegister(4);
          len = kernel->machine->ReadRegister(5);
          s = User2System(virtAddr, len);  // print maximum 100 characters
          printf("%s\n", s);

          kernel->machine->WriteRegister(2, 0);
          incrementPC();
          return;

          ASSERTNOTREACHED();

          break;
        }
        case SC_Add: {
          DEBUG(dbgSys, "Add " << kernel->machine->ReadRegister(4) << " + " << kernel->machine->ReadRegister(5) << "\n");

          /* Process SysAdd Systemcall*/
          int result;
          result = SysAdd(/* int op1 */ (int)kernel->machine->ReadRegister(4),
                          /* int op2 */ (int)kernel->machine->ReadRegister(5));

          DEBUG(dbgSys, "Add returning with " << result << "\n");
          /* Prepare Result */
          kernel->machine->WriteRegister(2, (int)result);

          /* Modify return point */
          incrementPC();
          return;

          ASSERTNOTREACHED();

          break;
        }
        case SC_Create: {
          int virtAddr;
          char* filename;
          DEBUG('a', "\n SC_Create call ...");
          DEBUG('a', "\n Reading virtual address of filename");
          // Lấy tham số tên tập tin từ thanh ghi r4
          virtAddr = kernel->machine->ReadRegister(4);
          DEBUG('a', "\n Reading filename.");

          filename = User2System(virtAddr, MAX_FILE_NAME_LENGTH);
          if (filename == NULL) {
            printf("\n Not enough memory in system");
            DEBUG('a', "\n Not enough memory in system");
            kernel->machine->WriteRegister(2, -1);  // trả về lỗi cho chương
            // trình người dùng
            delete filename;
            incrementPC();
            return;
          }
          DEBUG('a', "\n Finish reading filename.");
          // DEBUG('a',"\n File name : '"<<filename<<"'");
          //  Create file with size = 0
          //  Dùng đối tượng fileSystem của lớp OpenFile để tạo file,
          //  việc tạo file này là sử dụng các thủ tục tạo file của hệ điều
          //  hành Linux, chúng ta không quản ly trực tiếp các block trên
          //  đĩa cứng cấp phát cho file, việc quản ly các block của file
          //  trên ổ đĩa là một đồ án khác
          if (!kernel->fileSystem->Create(filename)) {
            printf("\n Error create file '%s'", filename);
            kernel->machine->WriteRegister(2, -1);
            delete filename;
            incrementPC();
            return;
          }

          printf("\n Create file '%s' successful\n", filename);
          kernel->machine->WriteRegister(2, 0);  // trả về cho chương trình người dùng thành công
          delete filename;
          incrementPC();

          return;
          ASSERTNOTREACHED();
          break;
        }
        case SC_Open: {
          int virtAddr, OpenFileID, type;
          char* filename;
          DEBUG('a', "\n SC_Open call ...");
          DEBUG('a', "\n Reading virtual address of filename");

          virtAddr = kernel->machine->ReadRegister(4);

          DEBUG('a', "\n Reading filename.");

          filename = User2System(virtAddr, MAX_FILE_NAME_LENGTH);
          if (filename == NULL) {
            printf("\n Not enough memory in system");
            DEBUG('a', "\n Not enough memory in system");
            kernel->machine->WriteRegister(2, -1);
            delete filename;
            incrementPC();
            return;
          }
          DEBUG('a', "\n Finish reading filename.");
          DEBUG('a', "\n Reading type (0 = Read write, 1 = Read-only).");
          type = kernel->machine->ReadRegister(5);

          if (type == 0) {
            if ((OpenFileID = kernel->fileSystem->OpenReadWrite(filename)) == -1) {
              printf("\n Error open file '%s'", filename);
              kernel->machine->WriteRegister(2, -1);
              delete filename;
              incrementPC();
              return;
            }
          } else if (type == 1) {
            if ((OpenFileID = kernel->fileSystem->OpenRead(filename)) == -1) {
              printf("\n Error open file '%s'", filename);
              kernel->machine->WriteRegister(2, -1);
              delete filename;
              incrementPC();
              return;
            }
          } else {
            printf("\n Invalid type");
            kernel->machine->WriteRegister(2, -1);
            delete filename;
            incrementPC();
            return;
          }

          printf("\n Open file '%s' for %s successful, OpenFileID: %d\n", filename, type ? "Read" : "Read Write", OpenFileID);
          kernel->machine->WriteRegister(2, OpenFileID);
          delete filename;
          incrementPC();

          return;
          ASSERTNOTREACHED();
          break;
        }
        case SC_Close: {
          int OpenFileID;
          DEBUG('a', "\n SC_Close call ...");
          DEBUG('a', "\n Reading OpenFileID argument");
          OpenFileID = kernel->machine->ReadRegister(4);

          if (kernel->fileSystem->CloseFile(OpenFileID) == -1) {
            printf("\n Error Close File, OpenFileID: %d", OpenFileID);
            kernel->machine->WriteRegister(2, -1);
            incrementPC();
            return;
          }

          printf("\n Close file successful, OpenFileID: %d\n", OpenFileID);
          kernel->machine->WriteRegister(2, 0);
          incrementPC();

          return;
          ASSERTNOTREACHED();
          break;
        }
        case SC_Read: {
          int virtAddr;  // read buffer address value
          int size;
          int OpenFileID;
          DEBUG('a', "\n SC_Read calls ...");
          DEBUG('a', "\n Reading arguments");

          virtAddr = kernel->machine->ReadRegister(4);
          size = kernel->machine->ReadRegister(5);
          OpenFileID = kernel->machine->ReadRegister(6);

          char* bufferInto = new char[size + 1];  // Read max is size otherwise numRead
          int numRead = 0;

          // handle normal file
          if (OpenFileID != ConsoleInputID && OpenFileID != ConsoleOutputID) {
            numRead = kernel->fileSystem->Read(bufferInto, size, OpenFileID);
            if (numRead == -1) {
              printf("Error Read File, OpenFileID: %d\n", OpenFileID);
              kernel->machine->WriteRegister(2, -1);
              incrementPC();
              return;
            }
          }

          // handle console input   --> ne
          if (OpenFileID == ConsoleInputID) {
            printf("Please type in:\n");  // for better debug
            SynchConsoleInput* input = new SynchConsoleInput(nullptr);
            for (int i = 0; i < size; ++i) {
              bufferInto[i] = input->GetChar();
              numRead++;
            }
            printf("debug\n");
            bufferInto[numRead] = '\0';
            delete input;
          }

          int copiedChar = System2User(virtAddr, numRead, bufferInto);
          // printf("\n value of copiedChar: %d\n", copiedChar); debug

          printf("Read file successful, OpenFileID: %d\n", OpenFileID);
          printf("value of buffer: %s\n", bufferInto);  // test
          printf("value of numRead: %d\n", numRead);    // debug

          kernel->machine->WriteRegister(2, numRead);
          incrementPC();
          return;
          ASSERTNOTREACHED();
          break;
        }

        case SC_Write: {
          int virtAddr;  // read buffer address value
          int size;
          int OpenFileID;
          DEBUG('a', "\n SC_Write calls ...");
          DEBUG('a', "\n Reading arguments");

          virtAddr = kernel->machine->ReadRegister(4);
          size = kernel->machine->ReadRegister(5);
          OpenFileID = kernel->machine->ReadRegister(6);

          char* buffer = User2System(virtAddr, size);  // Write max is size otherwise numWritten
          int numWritten = 0;

          if (size < 0) {
            printf("\n Error Write File, size cannot be negative");
            kernel->machine->WriteRegister(2, -1);
            incrementPC();
            return;
          }
          // handle normal file
          if (OpenFileID != 0 && OpenFileID != 1) {
            numWritten = kernel->fileSystem->Write(buffer, size, OpenFileID);
            if (numWritten == -1) {
              printf("\n Error Write File, OpenFileID: %d", OpenFileID);
              kernel->machine->WriteRegister(2, -1);
              incrementPC();
              return;
            }
          }

          // handle console output
          if (OpenFileID == ConsoleOutputID) {
            SynchConsoleOutput* output = new SynchConsoleOutput(nullptr);
            for (int i = 0; i < size; ++i) {
              if (buffer[i] == '\0') {
                break;
              }
              output->PutChar(buffer[i]);
              numWritten++;
            }
            delete output;
          }
          printf("\n Write file successful, OpenFileID: %d", OpenFileID);
          printf("\n Num of written chars: %d", numWritten);
          kernel->machine->WriteRegister(2, numWritten);
          incrementPC();

          return;
          ASSERTNOTREACHED();
          break;
        }
        case SC_Seek: {
          int pos;
          int OpenFileID;
          DEBUG('a', "\n SC_Seek calls ...");
          DEBUG('a', "\n Reading arguments");

          pos = kernel->machine->ReadRegister(4);
          OpenFileID = kernel->machine->ReadRegister(5);

          if (OpenFileID == ConsoleInputID || OpenFileID == ConsoleOutputID) {
            printf("\n Error Console File, OpenFileID: %d", OpenFileID);
            kernel->machine->WriteRegister(2, -1);
            incrementPC();
            return;
          }

          // handle normal file
          int actual_pos = kernel->fileSystem->Seek(pos, OpenFileID);
          if (actual_pos == -1) {
            printf("\n Error Seek File, OpenFileID: %d", OpenFileID);
            kernel->machine->WriteRegister(2, -1);
            incrementPC();
            return;
          }
          printf("\n Seek file successful, OpenFileID: %d", OpenFileID);
          kernel->machine->WriteRegister(2, actual_pos);
          incrementPC();

          return;
          ASSERTNOTREACHED();
          break;
        }

        case SC_Remove: {
          int virtAddr;
          DEBUG('a', "\n SC_Seek calls ...");
          DEBUG('a', "\n Reading arguments");

          virtAddr = kernel->machine->ReadRegister(4);

          char* filename;
          DEBUG('a', "\n Reading filename.");
          filename = User2System(virtAddr, MAX_FILE_NAME_LENGTH);

          if (filename == NULL) {
            printf("\n Not enough memory in system");
            DEBUG('a', "\n Not enough memory in system");
            kernel->machine->WriteRegister(2, -1);
            delete filename;
            incrementPC();
            return;
          }

          bool rmSuccess = kernel->fileSystem->Remove(filename);
          if (!rmSuccess) {
            printf("\n Error remove File: %s", filename);
            kernel->machine->WriteRegister(2, -1);
            incrementPC();
            return;
          }
          printf("\n Remove file %s successfully", filename);
          kernel->machine->WriteRegister(2, rmSuccess);
          incrementPC();

          return;
          ASSERTNOTREACHED();
          break;
        }
        case SC_OpenSocket: {
          SysOpenSocket();
          return;
          ASSERTNOTREACHED();
          break;
        }
        case SC_Connect: {
          SysConnect();
          return;
          ASSERTNOTREACHED();
          break;
        }
        case SC_Send: {
          SysSend();
          return;
          ASSERTNOTREACHED();
          break;
        }
        case SC_Receive: {
          SysReceive();
          return;
          ASSERTNOTREACHED();
          break;
        }
        case SC_CloseSocket: {
          SysCloseSocket();
          return;
          ASSERTNOTREACHED();
          break;
        }
        default: {
          printf("\nUnexpected user mode exception (%d %d)\n", which, type);
          kernel->interrupt->Halt();
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
