// utils.h

#ifndef UTILS_H
#define UTILS_H

void RawUser2System(int virtAddr, int size, char* buffer);
int StringUser2System(int virtAddr, int limit, char* buffer);
char* StringUser2System(int virtAddr);

void RawSystem2User(int virtAddr, int size, char* buffer);

void advancePC();
void setReturnCodeAndAdvancePC(int returnCode);

#endif  // UTILS_H
