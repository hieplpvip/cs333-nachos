// constant.h

#ifndef CONSTANT_H
#define CONSTANT_H

// Maximum number of user processes
const int NumProcesses = 10;

// Maximum number of user semaphores
const int NumSemaphores = 10;

// Maximum number of open files for each user process
const int NumOpenFilesPerProcess = 20;

// Maximum length of a file name
const int MaxFileNameLen = 256;

// Maximum length of an IP address
const int MaxIPAddressLen = 15;

// Size of the kernel thread's private execution stack.
// WATCH OUT IF THIS ISN'T BIG ENOUGH!!!!!
const int ThreadStackSize = (8 * 1024);  // in words

// Size of the user process's stack size
const int UserStackSize = 1024;  // in words

// Number of bytes per disk sector
const int SectorSize = 128;

// Number of sectors per disk track
const int SectorsPerTrack = 32;

// Number of tracks per disk
const int NumTracks = 32;

// Set the page size equal to the disk sector size, for simplicity
const int PageSize = 128;

// Number of pages of physical memory available on the simulated machine.
const int NumPhysPages = 128;

// Total amount of physical memory available on the simulated machine.
const int MemorySize = (NumPhysPages * PageSize);

// Number of TLB entries
const int TLBSize = 4;  // if there is a TLB, make it small

#endif  // CONSTANT_H
