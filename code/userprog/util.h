
#ifndef USERPROG_UTIL_H
#define USERPROG_UTIL_H

char* User2System(int virtAddr, int limit);
int System2User(int virtAddr, int len, char* buffer);
void incrementPC();

#endif
