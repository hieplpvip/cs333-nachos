// filesys.h
//	Data structures to represent the Nachos file system.
//
//	A file system is a set of files stored on disk, organized
//	into directories.  Operations on the file system have to
//	do with "naming" -- creating, opening, and deleting files,
//	given a textual file name.  Operations on an individual
//	"open" file (read, write, close) are to be found in the OpenFile
//	class (openfile.h).
//
//	We define two separate implementations of the file system.
//	The "STUB" version just re-defines the Nachos file system
//	operations as operations on the native UNIX file system on the machine
//	running the Nachos simulation.
//
//	The other version is a "real" file system, built on top of
//	a disk simulator.  The disk is simulated using the native UNIX
//	file system (in a file named "DISK").
//
//	In the "real" implementation, there are two key data structures used
//	in the file system.  There is a single "root" directory, listing
//	all of the files in the file system; unlike UNIX, the baseline
//	system does not provide a hierarchical directory structure.
//	In addition, there is a bitmap for allocating
//	disk sectors.  Both the root directory and the bitmap are themselves
//	stored as files in the Nachos file system -- this causes an interesting
//	bootstrap problem when the simulated disk is initialized.
//
// Copyright (c) 1992-1993 The Regents of the University of California.
// All rights reserved.  See copyright.h for copyright notice and limitation
// of liability and disclaimer of warranty provisions.

#ifndef FS_H
#define FS_H

#include "copyright.h"
#include "sysdep.h"
#include "openfile.h"

#define FDT_SIZE 20
#define RESERVED_FD 2

#ifdef FILESYS_STUB  // Temporarily implement file system calls as \
                     // calls to UNIX, until the real file system  \
                     // implementation is available
class FileSystem {
  struct OpenFileEntry {
    OpenFile *file;
    char *fileName;
    bool type;  // 0: read write, 1: read-only

    OpenFileEntry(OpenFile *_file, bool _type, char *_fileName) {
      file = _file;
      type = _type;
      fileName = _fileName;
    }

    ~OpenFileEntry() {
      if (file) {
        delete file;
      }
    }
  };

  struct OpenSocketEntry {
    int fd;

    OpenSocketEntry(int _fd) {
      fd = _fd;
    }

    ~OpenSocketEntry() {
      CloseSocket(fd);
    }
  };

  OpenFileEntry *FileTable[20];
  OpenSocketEntry *SocketTable[20];

public:
  FileSystem() {
    memset(FileTable, 0, sizeof(FileTable));
    memset(SocketTable, 0, sizeof(SocketTable));
  }

  // Ignore
  OpenFile *Open(char *name) {
    int fileDescriptor = OpenForReadWrite(name, FALSE);

    if (fileDescriptor == -1) {
      return NULL;
    }

    return new OpenFile(fileDescriptor);
  }

  // File
  bool Create(char *name) {
    int fileDescriptor = OpenForWrite(name);

    if (fileDescriptor == -1) {
      return FALSE;
    }

    Close(fileDescriptor);
    return TRUE;
  }

  int FindFreeSlot() {
    for (int i = RESERVED_FD; i < FDT_SIZE; ++i) {
      if (FileTable[i] == NULL) {
        return i;
      }
    }

    return -1;
  }

  int OpenRead(char *name) {
    int slot = FindFreeSlot();
    if (slot == -1) {
      return -1;
    }
    int fileDescriptor = OpenForRead(name);
    FileTable[slot] = new OpenFileEntry(new OpenFile(fileDescriptor), 1, name);
    return slot;
  }

  int OpenReadWrite(char *name) {
    int slot = FindFreeSlot();
    if (slot == -1) {
      return -1;
    }
    int fileDescriptor = OpenForReadWrite(name, FALSE);
    FileTable[slot] = new OpenFileEntry(new OpenFile(fileDescriptor), 0, name);
    return slot;
  }

  int CloseFile(int slot) {
    if (slot < RESERVED_FD || slot >= FDT_SIZE) return -1;
    if (!FileTable[slot]) {
      return -1;
    }
    delete FileTable[slot];
    FileTable[slot] = NULL;
    return 0;
  }

  bool Remove(char *name) {
    // check if a file is opened
    for (int i = 2; i < 20; ++i) {
      if (FileTable[i] == NULL) {
        continue;
      }
      if (strcmp(FileTable[i]->fileName, name) == 0) {
        return false;
      }
    }
    return Unlink(name) == 0;
  }

  int Read(char *buffer, int size, int slot) {
    if (slot < RESERVED_FD || slot >= FDT_SIZE || !FileTable[slot]) {
      return -1;
    }
    int numRead = FileTable[slot]->file->Read(buffer, size);
    return numRead;
  }

  int Write(char *buffer, int size, int slot) {
    if (slot <= 1 || slot >= 20 || !FileTable[slot]) {
      return -1;
    }
    if (FileTable[slot]->type) {
      printf("File ID %d cannot write", slot);
      return -1;
    }
    int numWritten = FileTable[slot]->file->Write(buffer, size);
    return numWritten;
  }

  int Seek(int position, int slot) {
    if (slot <= 1 || slot >= 20 || !FileTable[slot]) {
      return -1;
    }

    int actual_pos = FileTable[slot]->file->Seek(position);
    return actual_pos;
  }

  int FindFile(char *filename) {
  }

  // Socket
  int findFreeSlotSocket() {
    for (int i = RESERVED_FD; i < FDT_SIZE; ++i) {
      if (SocketTable[i] == NULL) {
        return i;
      }
    }
    return -1;
  }

  int CreateTCPSocket() {
    int slot = findFreeSlotSocket();
    if (slot == -1) {
      return -1;
    }
    int fd = OpenSocketInternet();
    if (fd < -1) {
      return -1;
    }
    SocketTable[slot] = new OpenSocketEntry(fd);
    return slot;
  }

  int ConnectTCPSocket(int slot, char *ip, int port) {
    if (slot < RESERVED_FD || slot >= FDT_SIZE || !SocketTable[slot]) {
      return -1;
    }
    return Connect(SocketTable[slot]->fd, ip, port);
  };

  int SendData(int slot, char *buffer, int len) {
    if (slot < RESERVED_FD || slot >= FDT_SIZE || !SocketTable[slot]) {
      return -1;
    }
    return Send(SocketTable[slot]->fd, buffer, len);
  }

  int ReceiveData(int slot, char *buffer, int len) {
    if (slot < RESERVED_FD || slot >= FDT_SIZE || !SocketTable[slot]) {
      return -1;
    }
    return Receive(SocketTable[slot]->fd, buffer, len);
  }

  int CloseTCPSocket(int slot) {
    if (slot < RESERVED_FD || slot >= FDT_SIZE || !SocketTable[slot]) {
      return -1;
    }
    delete SocketTable[slot];
    SocketTable[slot] = NULL;
    return 0;
  }
};

#else  // FILESYS
class FileSystem {
public:
  FileSystem(bool format);  // Initialize the file system.
                            // Must be called *after* "synchDisk"
                            // has been initialized.
                            // If "format", there is nothing on
                            // the disk, so initialize the directory
                            // and the bitmap of free blocks.

  bool Create(char* name, int initialSize);
  // Create a file (UNIX creat)

  OpenFile* Open(char* name);  // Open a file (UNIX open)

  bool Remove(char* name);  // Delete a file (UNIX unlink)

  void List();  // List all the files in the file system

  void Print();  // List all the files and their contents

private:
  OpenFile* freeMapFile;    // Bit map of free disk blocks,
                            // represented as a file
  OpenFile* directoryFile;  // "Root" directory -- list of
                            // file names, represented as a file
};

#endif  // FILESYS

#endif  // FS_H
