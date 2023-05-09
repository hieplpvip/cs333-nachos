// addrspace.cc
//	Routines to manage address spaces (executing user programs).
//
//	In order to run a user program, you must:
//
//	1. link with the -n -T 0 option
//	2. run coff2noff to convert the object file to Nachos format
//		(Nachos object code format is essentially just a simpler
//		version of the UNIX executable object code format)
//	3. load the NOFF file into the Nachos file system
//		(if you are using the "stub" file system, you
//		don't need to do this last step)
//
// Copyright (c) 1992-1996 The Regents of the University of California.
// All rights reserved.  See copyright.h for copyright notice and limitation
// of liability and disclaimer of warranty provisions.

#include "copyright.h"
#include "constant.h"
#include "main.h"
#include "addrspace.h"
#include "machine.h"
#include "noff.h"
#include "synch.h"

//----------------------------------------------------------------------
// SwapHeader
// 	Do little endian to big endian conversion on the bytes in the
//	object file header, in case the file was generated on a little
//	endian machine, and we're now running on a big endian machine.
//----------------------------------------------------------------------

static void
SwapHeader(NoffHeader *noffH) {
  noffH->noffMagic = WordToHost(noffH->noffMagic);
  noffH->code.size = WordToHost(noffH->code.size);
  noffH->code.virtualAddr = WordToHost(noffH->code.virtualAddr);
  noffH->code.inFileAddr = WordToHost(noffH->code.inFileAddr);
#ifdef RDATA
  noffH->readonlyData.size = WordToHost(noffH->readonlyData.size);
  noffH->readonlyData.virtualAddr =
      WordToHost(noffH->readonlyData.virtualAddr);
  noffH->readonlyData.inFileAddr =
      WordToHost(noffH->readonlyData.inFileAddr);
#endif
  noffH->initData.size = WordToHost(noffH->initData.size);
  noffH->initData.virtualAddr = WordToHost(noffH->initData.virtualAddr);
  noffH->initData.inFileAddr = WordToHost(noffH->initData.inFileAddr);
  noffH->uninitData.size = WordToHost(noffH->uninitData.size);
  noffH->uninitData.virtualAddr = WordToHost(noffH->uninitData.virtualAddr);
  noffH->uninitData.inFileAddr = WordToHost(noffH->uninitData.inFileAddr);

#ifdef RDATA
  DEBUG(dbgAddr, "code = " << noffH->code.size << " readonly = " << noffH->readonlyData.size << " init = " << noffH->initData.size << " uninit = " << noffH->uninitData.size << "\n");
#endif
}

//----------------------------------------------------------------------
// AddrSpace::AddrSpace
// 	Create an address space to run a user program.
//----------------------------------------------------------------------

AddrSpace::AddrSpace() {
  pageTable = NULL;
  numPages = 0;
  argCount = 0;
  argSize = 0;
}

//----------------------------------------------------------------------
// AddrSpace::~AddrSpace
// 	Deallocate an address space.
//----------------------------------------------------------------------

AddrSpace::~AddrSpace() {
  if (pageTable) {
    // Free up pages
    for (unsigned int i = 0; i < numPages; i++) {
      if (pageTable[i].valid) {
        kernel->gPhysPageBitMap->Clear(pageTable[i].physicalPage);
      }
    }

    delete pageTable;
  }
}

//----------------------------------------------------------------------
// AddrSpace::Load
// 	Load a user program into memory from a file.
//
//	Assumes the object code file is in NOFF format.
//
//	"fileName" is the file containing the object code to load into memory
//----------------------------------------------------------------------

bool AddrSpace::Load(const char *fileName, int argc, char const *const *argv) {
  OpenFile *executable = kernel->fileSystem->Open(fileName);
  NoffHeader noffH;
  unsigned int size;

  if (executable == NULL) {
    DEBUG(dbgAddr, "Unable to open file " << fileName);
    return FALSE;
  }

  executable->ReadAt((char *)&noffH, sizeof(noffH), 0);
  if ((noffH.noffMagic != NOFFMAGIC) &&
      (WordToHost(noffH.noffMagic) == NOFFMAGIC))
    SwapHeader(&noffH);
  ASSERT(noffH.noffMagic == NOFFMAGIC);

  argCount = argc;
  argSize = 0;
  if (argc > 0) {
    argSize += 4 * (argc + 1);
    for (int i = 0; i < argc; i++) {
      argSize += strlen(argv[i]) + 1;
    }
  }
  argSize = divRoundUp(argSize, 4) * 4;

#ifdef RDATA
  // how big is address space?
  size = noffH.code.size + noffH.readonlyData.size + noffH.initData.size +
         noffH.uninitData.size + UserStackSize + argSize;
  // we need to increase the size
  // to leave room for the stack
#else
  // how big is address space?
  size = noffH.code.size + noffH.initData.size + noffH.uninitData.size + UserStackSize + argSize;
  // we need to increase the size
  // to leave room for the stack
#endif
  numPages = divRoundUp(size, PageSize);
  size = numPages * PageSize;

  ASSERT(numPages <= NumPhysPages);  // check we're not trying
                                     // to run anything too big --
                                     // at least until we have
                                     // virtual memory

  DEBUG(dbgAddr, "Initializing address space: " << numPages << ", " << size);

  if (!InitPageTable()) {
    DEBUG(dbgAddr, "Failed to initialize page table");
    numPages = 0;
    delete executable;
    return FALSE;
  }

  // then, copy in the code and data segments into memory
  // Note: this code assumes that virtual address = physical address
  if (noffH.code.size > 0) {
    DEBUG(dbgAddr, "Initializing code segment.");
    DEBUG(dbgAddr, noffH.code.virtualAddr << ", " << noffH.code.size);
    CopyFromFile(executable, noffH.code.inFileAddr, noffH.code.size, noffH.code.virtualAddr);
  }
  if (noffH.initData.size > 0) {
    DEBUG(dbgAddr, "Initializing data segment.");
    DEBUG(dbgAddr, noffH.initData.virtualAddr << ", " << noffH.initData.size);
    CopyFromFile(executable, noffH.initData.inFileAddr, noffH.initData.size, noffH.initData.virtualAddr);
  }

#ifdef RDATA
  if (noffH.readonlyData.size > 0) {
    DEBUG(dbgAddr, "Initializing read only data segment.");
    DEBUG(dbgAddr, noffH.readonlyData.virtualAddr << ", " << noffH.readonlyData.size);
    CopyFromFile(executable, noffH.readonlyData.inFileAddr, noffH.readonlyData.size, noffH.readonlyData.virtualAddr);
  }
#endif

  if (argc > 0) {
    DEBUG(dbgAddr, "Copying " << argc << " arguments to memory.");
    int argAddr = size - argSize;
    int strAddr = argAddr + 4 * (argc + 1);
    unsigned int physAddr;
    for (int i = 0; i < argc; i++) {
      int strLen = strlen(argv[i]) + 1;

      // write argument address
      ASSERT(Translate(argAddr, &physAddr, TRUE) == NoException);
      *(unsigned int *)&kernel->machine->mainMemory[physAddr] = WordToMachine((unsigned int)strAddr);

      // write argument string
      CopyFromBuffer(argv[i], strLen, strAddr);

      argAddr += 4;
      strAddr += strLen;
    }

    // zero terminate argument list
    ASSERT(Translate(argAddr, &physAddr, TRUE) == NoException);
    kernel->machine->mainMemory[physAddr] = 0;
  }

  delete executable;  // close file
  return TRUE;        // success
}

//----------------------------------------------------------------------
// AddrSpace::Execute
// 	Run a user program using the current thread
//
//      The program is assumed to have already been loaded into
//      the address space
//
//----------------------------------------------------------------------

void AddrSpace::Execute() {
  ASSERT(kernel->currentThread->space == NULL);
  kernel->currentThread->space = this;

  this->InitRegisters();  // set the initial register values
  this->RestoreState();   // load page table register

  kernel->machine->Run();  // jump to the user program

  ASSERTNOTREACHED();  // machine->Run never returns;
                       // the address space exits
                       // by doing the syscall "exit"
}

//----------------------------------------------------------------------
// AddrSpace::InitPageTable
// 	Set up the translation from program memory to physical memory.
//	Assume `numPages` are set by Load().
//	Pages are zeroed out.
//----------------------------------------------------------------------

bool AddrSpace::InitPageTable() {
  ASSERT(pageTable == NULL);

  kernel->addrLock->P();

  if (numPages > (unsigned int)kernel->gPhysPageBitMap->NumClear()) {
    DEBUG(dbgAddr, "Not enough free pages");
    kernel->addrLock->V();
    return FALSE;
  }

  pageTable = new TranslationEntry[numPages];
  for (unsigned int i = 0; i < numPages; i++) {
    pageTable[i].virtualPage = i;
    pageTable[i].physicalPage = kernel->gPhysPageBitMap->FindAndSet();
    pageTable[i].valid = TRUE;
    pageTable[i].use = FALSE;
    pageTable[i].dirty = FALSE;
    pageTable[i].readOnly = FALSE;

    // zero out the physical frame
    bzero(&kernel->machine->mainMemory[pageTable[i].physicalPage * PageSize], PageSize);
  }

  kernel->addrLock->V();
  return TRUE;
}

//----------------------------------------------------------------------
// AddrSpace::InitRegisters
// 	Set the initial values for the user-level register set.
//
// 	We write these directly into the "machine" registers, so
//	that we can immediately jump to user code.  Note that these
//	will be saved/restored into the currentThread->userRegisters
//	when this thread is context switched out.
//----------------------------------------------------------------------

void AddrSpace::InitRegisters() {
  Machine *machine = kernel->machine;
  int i;

  for (i = 0; i < NumTotalRegs; i++)
    machine->WriteRegister(i, 0);

  // Initial program counter -- must be location of "Start", which
  //  is assumed to be virtual address zero
  machine->WriteRegister(PCReg, 0);

  // Need to also tell MIPS where next instruction is, because
  // of branch delay possibility
  // Since instructions occupy four bytes each, the next instruction
  // after start will be at virtual address four.
  machine->WriteRegister(NextPCReg, 4);

  // Set the stack register to the end of the address space, where we
  // allocated the stack; but subtract off a bit, to make sure we don't
  // accidentally reference off the end!
  int stackReg = numPages * PageSize - argSize - 16;
  machine->WriteRegister(StackReg, stackReg);
  DEBUG(dbgAddr, "Initializing stack pointer: " << stackReg);

  // Pass argc and argv to the program
  if (argCount > 0) {
    kernel->machine->WriteRegister(4, argCount);
    kernel->machine->WriteRegister(5, numPages * PageSize - argSize);
  }
}

//----------------------------------------------------------------------
// AddrSpace::SaveState
// 	On a context switch, save any machine state, specific
//	to this address space, that needs saving.
//
//	For now, don't need to save anything!
//----------------------------------------------------------------------

void AddrSpace::SaveState() {}

//----------------------------------------------------------------------
// AddrSpace::RestoreState
// 	On a context switch, restore the machine state so that
//	this address space can run.
//
//      For now, tell the machine where to find the page table.
//----------------------------------------------------------------------

void AddrSpace::RestoreState() {
  kernel->machine->pageTable = pageTable;
  kernel->machine->pageTableSize = numPages;
}

void AddrSpace::CopyFromFile(OpenFile *file, int inFileAddr, int size, int virtualAddr) {
  while (size > 0) {
    int vpn = virtualAddr / PageSize;
    int offset = virtualAddr % PageSize;
    int pfn = pageTable[vpn].physicalPage;
    int amount = min(PageSize - offset, size);

    // Set the use and dirty bits
    pageTable[vpn].use = TRUE;
    pageTable[vpn].dirty = TRUE;

    file->ReadAt(&(kernel->machine->mainMemory[pfn * PageSize + offset]), amount, inFileAddr);

    size -= amount;
    virtualAddr += amount;
    inFileAddr += amount;
  }
}

void AddrSpace::CopyFromBuffer(const char *buf, int size, int virtualAddr) {
  while (size > 0) {
    int vpn = virtualAddr / PageSize;
    int offset = virtualAddr % PageSize;
    int pfn = pageTable[vpn].physicalPage;
    int amount = min(PageSize - offset, size);

    // Set the use and dirty bits
    pageTable[vpn].use = TRUE;
    pageTable[vpn].dirty = TRUE;

    memcpy(&(kernel->machine->mainMemory[pfn * PageSize + offset]), buf, amount);

    size -= amount;
    virtualAddr += amount;
    buf += amount;
  }
}

//----------------------------------------------------------------------
// AddrSpace::Translate
//  Translate the virtual address in _vaddr_ to a physical address
//  and store the physical address in _paddr_.
//  The flag _isReadWrite_ is false (0) for read-only access; true (1)
//  for read-write access.
//  Return any exceptions caused by the address translation.
//----------------------------------------------------------------------
ExceptionType
AddrSpace::Translate(unsigned int vaddr, unsigned int *paddr, int isReadWrite) {
  TranslationEntry *pte;
  int pfn;
  unsigned int vpn = vaddr / PageSize;
  unsigned int offset = vaddr % PageSize;

  if (vpn >= numPages) {
    return AddressErrorException;
  }

  pte = &pageTable[vpn];

  if (isReadWrite && pte->readOnly) {
    return ReadOnlyException;
  }

  pfn = pte->physicalPage;

  // if the pageFrame is too big, there is something really wrong!
  // An invalid translation was loaded into the page table or TLB.
  if (pfn >= NumPhysPages) {
    DEBUG(dbgAddr, "Illegal physical page " << pfn);
    return BusErrorException;
  }

  pte->use = TRUE;  // set the use, dirty bits

  if (isReadWrite)
    pte->dirty = TRUE;

  *paddr = pfn * PageSize + offset;

  ASSERT((*paddr < MemorySize));

  // cerr << " -- AddrSpace::Translate(): vaddr: " << vaddr <<
  //   ", paddr: " << *paddr << "\n";

  return NoException;
}
