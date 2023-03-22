
#ifndef USERPROG_UTIL_H
#define USERPROG_UTIL_H

void RawUser2System(int virtAddr, int size, char* buffer);
int StringUser2System(int virtAddr, int limit, char* buffer);
char* StringUser2System(int virtAddr);

void RawSystem2User(int virtAddr, int size, char* buffer);

void incrementPC();
void setReturnCodeAndIncrementPC(int returnCode);

#endif
