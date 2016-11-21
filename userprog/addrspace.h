// addrspace.h 
//	Data structures to keep track of executing user programs 
//	(address spaces).
//
//	For now, we don't keep any information about address spaces.
//	The user level CPU state is saved and restored in the thread
//	executing the user program (see thread.h).
//
// Copyright (c) 1992-1993 The Regents of the University of California.
// All rights reserved.  See copyright.h for copyright notice and limitation 
// of liability and disclaimer of warranty provisions.

#ifndef ADDRSPACE_H
#define ADDRSPACE_H

#include "copyright.h"
#include "filesys.h"


typedef int OpenFileId;	

#define UserStackSize		1024 	// increase this as necessary!

class AddrSpace {
  public:
    AddrSpace(AddrSpace *copySpace);
    AddrSpace(OpenFile *executable);	// Create an address space,
					// initializing it with the program
					// stored in the file "executable"
    ~AddrSpace();			// De-allocate an address space

    void InitRegisters();		// Initialize user-level CPU registers,
					// before jumping to user code

    void SaveState();			// Save/restore address space-specific
    void RestoreState();		// info on a context switch 
    
   
#ifdef CHANGED
    OpenFileId fileOpen(char* fileName); 
    OpenFile* readWrite(OpenFileId fileId);
    void fileClose(OpenFileId fileId);
    int isConsoleFile(OpenFile* file);
    OpenFileId dupFd(int fd);
    
    OpenFile* stdIn;   //Cookie corresponding to stdIn, not a "real" OpenFile object
    OpenFile* stdOut;  //Cookie corresponding to stdOut, not a "real" OpenFile object
    OpenFile* fileVector [20]; //Maps file descriptors (indices) to OpenFile objects
    int mySpaceId;
    int parentThreadPtr;
    bool checkpoint;
    bool failed;
    unsigned int numPages;		// Number of pages in the virtual 
    char* fileName;          
    int diskSectors [2048];
#endif
#ifndef USE_TLB
    TranslationEntry *pageTable;	// Assume linear page table translation
#endif					// for now!

  private:
    int convertVirtualtoPhysical(int virtualAddr);
    
};

#endif // ADDRSPACE_H
