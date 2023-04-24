// sem.h

#ifndef SEM_H
#define SEM_H

class Semaphore;

class Sem {
private:
  char* name;
  Semaphore* sem;

public:
  Sem(const char* _name, int initialValue);
  ~Sem();

  // Conduct the waiting function
  void wait();

  // Release semaphore
  void signal();

  // Return the semaphore name
  const char* getName() const;
};

#endif  // SEM_H
