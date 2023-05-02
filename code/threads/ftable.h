// ftable.h

#ifndef FTABLE_H
#define FTABLE_H

#include "constant.h"

class FileTable {
private:
  // Map from local (per process) to global file descriptor
  int table[NumOpenFilesPerProcess];

  int findFreeSlot() const;

public:
  FileTable();
  ~FileTable();

  bool Create(char *name);
  int Open(char *name, int mode);
  bool Close(int slot);
  bool Remove(char *name);
  int Read(int slot, char *buffer, int count);
  int Write(int slot, char *buffer, int count);
  int Seek(int slot, int pos);

  int CreateTCPSocket();
  int ConnectTCPSocket(int slot, char *ip, int port);
  int SendData(int slot, char *buffer, int count);
  int ReceiveData(int slot, char *buffer, int count);
  bool CloseTCPSocket(int slot);
};

#endif  // FTABLE_H
