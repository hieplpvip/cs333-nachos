// pcb.h

#ifndef PCB_H
#define PCB_H

class Semaphore;
class Thread;

class PCB {
private:
  char* filename;
  Semaphore* joinsem;
  Semaphore* exitsem;
  int processID;
  int parentID;
  int exitCode;

public:
  Thread* thread;

  PCB(int pid, int parentId);
  ~PCB();

  // Start the process
  int Exec(int argc, char** argv);

  // Return the PID of the current process
  int GetProcessID() const;

  // Return the PID of the parent process
  int GetParentID() const;

  // The parent process wait for the child process finishes
  void JoinWait();

  // The child process notice the parent process
  void JoinRelease();

  // The child process finishes
  void ExitWait();

  // The parent process accept to exit the child process
  void ExitRelease();

  // Set the exit code for the process
  void SetExitCode(int exitCode);

  // Return the exitcode
  int GetExitCode() const;

  // Set the file name (can only be called once)
  void setFileName(const char* name);

  // Return the file name
  const char* GetFileName() const;
};

#endif  // PCB_H
