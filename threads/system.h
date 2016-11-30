// system.h 
//	All global variables used in Nachos are defined here.
//
// Copyright (c) 1992-1993 The Regents of the University of California.
// All rights reserved.  See copyright.h for copyright notice and limitation 
// of liability and disclaimer of warranty provisions.

#ifndef SYSTEM_H
#define SYSTEM_H

#include "copyright.h"
#include "utility.h"
#include "thread.h"
#include "synch.h"
#include "scheduler.h"
#include "interrupt.h"
#include "stats.h"
#include "slicingTimer.h"
#include "timer.h"
#include "bitmap.h"
#include "syscall.h"
#include "joinlist.h"
#include <new>

// Initialization and cleanup routines
extern void Initialize(int argc, char **argv); 	// Initialization,
						// called before anything else
extern void Cleanup();				// Cleanup, called when
						// Nachos is done.

extern Thread *currentThread;			// the thread holding the CPU
extern Thread *threadToBeDestroyed;  		// the thread that just finished
extern Scheduler *scheduler;			// the ready list
extern Interrupt *interrupt;			// interrupt status
extern Statistics *stats;			// performance metrics

#ifdef CHANGED
#ifdef USER_PROGRAM
#include "machine.h"
#include "synchconsole.h"
#include "synchdisk.h"
#include "faultdata.h"

extern SlicingTimer *timer;     //Timer that forces interrupts every 100 ticks
extern SpaceId spaceId;         //Process id for new processes
extern Semaphore *spaceIdSem;   //Synchronizes use of spaceId
extern JoinList *joinList;      //Linked list maintaining join related info for all parent/children pairs
extern Semaphore *joinSem;      //Synchronizes use of joinList
extern Semaphore *forkSem;      //Prevents a parent from executing before its child is finished initializing its address space
extern Machine* machine;	//user program memory and registers
extern SynchConsole* sConsole;  //Console class to unify I/O operations
extern BitMap *memMap;          //global memory map
extern BitMap *diskMap;         //Global disk sectors map
extern Lock *bitLock;           //Synchronizes access to the memMap
extern Lock *diskBitLock;       //Synchronizes access to the diskMap
extern Lock *forkExec;          //Synchronizes access to the Fork and Exec syscalls
extern Lock *stdOut;            //Synchronizes writes to the console
extern Lock *atomicWrite;       //Synchronizes writes to files
extern Lock *vmInfoLock;        //Synchronizes access to any valid bit and faultinfo
extern Lock *faultLock;         //Synchronizes faulting behavior  
extern Lock *diskSectorsLock;   //Synchronizes access to any thread's diskSectors
extern Lock *killLock;          //Synchronizes with read-only stuff and killing a thread
extern Lock *readOnlyLock;      //Synchronizes access to read only fix functions
extern SynchDisk *megaDisk;     //I think the name explains it all
extern FaultData **faultInfo;   //Keeps track of necessary info for pages in memory with respect to a pageFault
extern int clockPos;


#endif
#endif

#ifdef FILESYS_NEEDED 		// FILESYS or FILESYS_STUB 
#include "filesys.h"
extern FileSystem  *fileSystem;
#endif

#ifdef FILESYS
#include "synchdisk.h"
extern SynchDisk   *synchDisk;
#endif

#ifdef NETWORK
#include "post.h"
extern PostOffice* postOffice;
#endif

#endif // SYSTEM_H
