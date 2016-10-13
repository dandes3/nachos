// exception.cc 
//	Entry point into the Nachos kernel from user programs.
//	There are two kinds of things that can cause control to
//	transfer back to here from user code:
//
//	syscall -- The user code explicitly requests to call a procedure
//	in the Nachos kernel.  Right now, the only function we support is
//	"Halt".
//
//	exceptions -- The user code does something that the CPU can't handle.
//	For instance, accessing memory that doesn't exist, arithmetic errors,
//	etc.  
//
//	Interrupts (which can also cause control to transfer from user
//	code into the Nachos kernel) are handled elsewhere.
//
// For now, this only handles the Halt() system call.
// Everything else core dumps.
//
// Copyright (c) 1992-1993 The Regents of the University of California.
// All rights reserved.  See copyright.h for copyright notice and limitation 
// of liability and disclaimer of warranty provisions.

#include "copyright.h"
#include "system.h"
#include "syscall.h"
#include "filesys.h"

#ifdef USE_TLB

//----------------------------------------------------------------------
// HandleTLBFault
//      Called on TLB fault. Note that this is not necessarily a page
//      fault. Referenced page may be in memory.
//
//      If free slot in TLB, fill in translation info for page referenced.
//
//      Otherwise, select TLB slot at random and overwrite with translation
//      info for page referenced.
//
//----------------------------------------------------------------------

void
HandleTLBFault(int vaddr)
{
  int vpn = vaddr / PageSize;
  int victim = Random() % TLBSize;
  int i;

  stats->numTLBFaults++;

  // First, see if free TLB slot
  for (i=0; i<TLBSize; i++)
    if (machine->tlb[i].valid == false) {
      victim = i;
      break;
    }

  // Otherwise clobber random slot in TLB

  machine->tlb[victim].virtualPage = vpn;
  machine->tlb[victim].physicalPage = vpn; // Explicitly assumes 1-1 mapping
  machine->tlb[victim].valid = true;
  machine->tlb[victim].dirty = false;
  machine->tlb[victim].use = false;
  machine->tlb[victim].readOnly = false;
}

#endif

//----------------------------------------------------------------------
// ExceptionHandler
// 	Entry point into the Nachos kernel.  Called when a user program
//	is executing, and either does a syscall, or generates an addressing
//	or arithmetic exception.
//
// 	For system calls, the following is the calling convention:
//
// 	system call code -- r2
//		arg1 -- r4
//		arg2 -- r5
//		arg3 -- r6
//		arg4 -- r7
//
//	The result of the system call, if any, must be put back into r2. 
//
// And don't forget to increment the pc before returning. (Or else you'll
// loop making the same system call forever!
//
//	"which" is the kind of exception.  The list of possible exceptions 
//	are in machine.h.
//----------------------------------------------------------------------

void ReadArg(char* result, int size);

void
ExceptionHandler(ExceptionType which)
{
    fprintf(stderr, "Fuck you\n");
    
    int type = machine->ReadRegister(2);
    int size;
    char* intoBuf, *fromBuf;
    OpenFileId fileId;
    OpenFile* readFile, *writeFile;
    
    char* arg;
    
    switch (which) {
      case SyscallException:
	    switch (type) {
          case SC_Halt:
             DEBUG('a', "Shutdown, initiated by user program.\n");
             interrupt->Halt();
             break;
             
          case SC_Create:
             DEBUG('a', "Create entered\n");
             arg = new(std::nothrow) char[128];
             ReadArg(arg, 128);
             //fprintf(stderr, "%s\n", arg);
             DEBUG('a', "Create entered\n");
             fileSystem -> Create(arg, -1);
             break;
             
          case SC_Open:
             DEBUG('a', "Open entered\n");
             ReadArg(arg, 128);
             fileId = currentThread -> space -> fileOpen(arg);
             machine -> WriteRegister(2, fileId);
             break;
             
          case SC_Close:
              DEBUG('a', "Close entered\n");
              currentThread -> space -> fileClose(machine->ReadRegister(4));
              break;
              
          case SC_Read:
              DEBUG('a', "Read entered\n");
              intoBuf = (char*) machine -> ReadRegister(4);
              size = machine -> ReadRegister(5);
              fileId = machine->ReadRegister(6);
              
              readFile = currentThread -> space -> readWrite(fileId);
              
              if (readFile == NULL)
                  machine -> WriteRegister(2, -1);
              
              else
                  machine -> WriteRegister(2, readFile -> Read(intoBuf, size));
              break;
              
          case SC_Write:
              DEBUG('a', "Write entered\n");
              fromBuf = (char*) machine -> ReadRegister(4);
              size = machine -> ReadRegister(5);
              fileId = machine->ReadRegister(6);
              
              writeFile = currentThread -> space -> readWrite(fileId);
              
              if (readFile != NULL)
                  writeFile -> Write(fromBuf, size);
              break;
              
          default:
	         printf("Undefined SYSCALL %d\n", type);
	         ASSERT(false);
	}
#ifdef USE_TLB
      case PageFaultException:
	HandleTLBFault(machine->ReadRegister(BadVAddrReg));
	break;
#endif
      default: ;
    }
    
    
}

void ReadArg(char* result, int size){
    
    int location;
    
    location = machine->ReadRegister(4);
    //result = new(std::nothrow) char[size];
    for (int i = 0; i < size - 1; i++){
        //fprintf(stderr, "%c", machine->mainMemory[location]);
        if ((result[i] = machine->mainMemory[location++]) == '\0')
            break;
    }
    
    result[size - 1] = '\0';
    
}


