// sem.cc

#include "sem.h"
#include "synch.h"

Sem::Sem(const char* _name, int initialValue) {
  if (_name) {
    name = new char[strlen(_name) + 1];
    strcpy(name, _name);
  } else {
    name = NULL;
  }

  sem = new Semaphore(name, initialValue);
}

Sem::~Sem() {
  delete sem;
  if (name) {
    delete[] name;
  }
}

void Sem::wait() {
  sem->P();
}

void Sem::signal() {
  sem->V();
}

const char* Sem::getName() const {
  return name;
}
