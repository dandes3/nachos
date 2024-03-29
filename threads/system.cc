// system.cc 
//	Nachos initialization and cleanup routines.
//
// Copyright (c) 1992-1993 The Regents of the University of California.
// All rights reserved.  See copyright.h for copyright notice and limitation 
// of liability and disclaimer of warranty provisions.

#include "copyright.h"
#include "system.h"

// This defines *all* of the global data structures used by Nachos.
// These are all initialized and de-allocated by this file.

Thread *currentThread;			// the thread we are running now
Thread *threadToBeDestroyed;  		// the thread that just finished
Scheduler *scheduler;			// the ready list
Interrupt *interrupt;			// interrupt status
Statistics *stats;			// performance metrics
SlicingTimer *timer;				// the hardware timer device,
//Timer *timer;					// for invoking context switches

#ifdef FILESYS_NEEDED
FileSystem  *fileSystem;
#endif

#ifdef FILESYS
SynchDisk   *synchDisk;
#endif

#ifdef CHANGED
#ifdef USER_PROGRAM	// requires either FILESYS or FILESYS_STUB
Machine *machine;	// user program memory and registers
SynchConsole *sConsole;
BitMap *memMap;
BitMap *diskMap;
Lock *bitLock;
Lock *diskBitLock;
SpaceId spaceId;
Semaphore *forkSem;
Semaphore *joinSem;
Semaphore *spaceIdSem;
JoinList *joinList;
Lock *forkExec;
Lock *stdOut;
Lock *atomicWrite;
Lock *vmInfoLock;
Lock *faultLock;
SynchDisk *megaDisk;
FaultData **faultInfo;
int clockPos;
#endif
#endif

#ifdef NETWORK
char diskname[50];
PostOffice *postOffice;
#endif

// External definition, to allow us to take a pointer to this function
extern void Cleanup();


//----------------------------------------------------------------------
// TimerInterruptHandler
// 	Interrupt handler for the timer device.  The timer device is
//	set up to interrupt the CPU periodically (once every TimerTicks).
//	This routine is called each time there is a timer interrupt,
//	with interrupts disabled.
//
//	Note that instead of calling Yield() directly (which would
//	suspend the interrupt handler, not the interrupted thread
//	which is what we wanted to context switch), we set a flag
//	so that once the interrupt handler is done, it will appear as 
//	if the interrupted thread called Yield at the point it is 
//	was interrupted.
//
//	"dummy" is because every interrupt handler takes one argument,
//		whether it needs it or not.
//----------------------------------------------------------------------
static void
TimerInterruptHandler(int )
{
    if (interrupt->getStatus() != IdleMode)
	interrupt->YieldOnReturn();
}

//----------------------------------------------------------------------
// Initialize
// 	Initialize Nachos global data structures.  Interpret command
//	line arguments in order to determine flags for the initialization.  
// 
//	"argc" is the number of command line arguments (including the name
//		of the command) -- ex: "nachos -d +" -> argc = 3 
//	"argv" is an array of strings, one for each command line argument
//		ex: "nachos -d +" -> argv = {"nachos", "-d", "+"}
//----------------------------------------------------------------------
void
Initialize(int argc, char **argv)
{
    int argCount;
    char *debugArgs = (char *)""; 
    bool randomYield = false;

#ifdef USER_PROGRAM
    bool debugUserProg = false;	// single step user program
#endif
#ifdef FILESYS_NEEDED
    bool format = false;	// format disk
#endif
#ifdef NETWORK
    double rely = 1;		// network reliability
    int netname = 0;		// UNIX socket name
#endif
    
    for (argc--, argv++; argc > 0; argc -= argCount, argv += argCount) {
	argCount = 1;
	if (!strcmp(*argv, "-d")) {
	    if (argc == 1)
		debugArgs = (char *) "+";	// turn on all debug flags
	    else {
	    	debugArgs = *(argv + 1);
	    	argCount = 2;
	    }
	} else if (!strcmp(*argv, "-rs")) {
	    ASSERT(argc > 1);
	    RandomInit(atoi(*(argv + 1)));	// initialize pseudo-random
						// number generator
	    randomYield = true;
	    argCount = 2;
	}


#ifdef USER_PROGRAM
	if (!strcmp(*argv, "-s"))
	    debugUserProg = true;
#endif
#ifdef FILESYS_NEEDED
	if (!strcmp(*argv, "-f"))
	    format = true;
#endif
#ifdef NETWORK
	if (!strcmp(*argv, "-n")) {
	    ASSERT(argc > 1);
	    rely = atof(*(argv + 1));
	    argCount = 2;
	} else if (!strcmp(*argv, "-m")) {
	    ASSERT(argc > 1);
	    netname = atoi(*(argv + 1));
	    argCount = 2;
	}
#endif
    }

    DebugInit(debugArgs);			// initialize DEBUG messages
    stats = new(std::nothrow) Statistics();			// collect statistics
    interrupt = new(std::nothrow) Interrupt;			// start up interrupt handling
    scheduler = new(std::nothrow) Scheduler();		// initialize the ready queue
    				// start the timer (if needed)
    threadToBeDestroyed = NULL;

    // We didn't explicitly allocate the current thread we are running in.
    // But if it ever tries to give up the CPU, we better have a Thread
    // object to save its state. 
    currentThread = new(std::nothrow) Thread("main");		
    currentThread->setStatus(RUNNING);

    interrupt->Enable();
    CallOnUserAbort(Cleanup);			// if user hits ctl-C
    
#ifdef USER_PROGRAM
#ifdef CHANGED
    machine = new(std::nothrow) Machine(debugUserProg);	// this must come first
    sConsole = new(std::nothrow) SynchConsole(NULL, NULL);
    memMap = new(std::nothrow) BitMap(NumPhysPages);
    diskMap = new(std::nothrow) BitMap(SectorsPerTrack * NumTracks);
    diskBitLock = new(std::nothrow) Lock("diskBitLock");
    bitLock = new(std::nothrow) Lock("bitLock");
    spaceId = 1;
    forkSem = new(std::nothrow) Semaphore("forkSem", 0);
    joinSem = new(std::nothrow) Semaphore("joinSem", 1);
    spaceIdSem = new(std::nothrow) Semaphore("spaceIdSem", 1);
    joinList =  new(std::nothrow) JoinList();
    forkExec = new(std::nothrow) Lock("forkExec");
    stdOut = new(std::nothrow) Lock("stdOut");
    atomicWrite = new(std::nothrow) Lock("atomicWrite");
    vmInfoLock = new(std::nothrow) Lock("vmInfoLock");
    faultLock = new(std::nothrow) Lock("faultLock");
    timer = new(std::nothrow) SlicingTimer(TimerInterruptHandler, 0);
    megaDisk = new(std::nothrow) SynchDisk("megaDisk");
    faultInfo = new(std::nothrow) FaultData*[NumPhysPages];
    clockPos = 0;
    for (int i = 0; i < NumPhysPages; i++)
        faultInfo[i] = NULL;
    
#endif
#endif

#ifdef FILESYS
    synchDisk = new(std::nothrow) SynchDisk("DISK");
#endif

#ifdef FILESYS_NEEDED
    fileSystem = new(std::nothrow) FileSystem(format);
#endif

#ifdef NETWORK
    postOffice = new(std::nothrow) PostOffice(netname, rely, 10);
#endif


}

//----------------------------------------------------------------------
// Cleanup
// 	Nachos is halting.  De-allocate global data structures.
//----------------------------------------------------------------------
void
Cleanup()
{
    printf("\nCleaning up...\n");
#ifdef NETWORK
    delete postOffice;
#endif
    
#ifdef USER_PROGRAM
    delete machine;
    //delete memMap;
#endif

#ifdef FILESYS_NEEDED
    delete fileSystem;
#endif

#ifdef FILESYS
    delete synchDisk;
#endif
    
    delete timer;
    delete scheduler;
    delete interrupt;
    
    Exit(0);
}

