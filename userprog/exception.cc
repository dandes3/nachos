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

void
ExceptionHandler(ExceptionType which)
{
    int type = machine->ReadRegister(2);
    char* arg;
    
    switch (which) {
      case SyscallException:
	    switch (type) {
          case SC_Halt:
             DEBUG('a', "Shutdown, initiated by user program.\n");
             interrupt->Halt();
             
          case SC_Create:
             ReadArg(arg, 128);
             Create(arg);
             
          case SC_Open:
             ReadArg(arg, 128);
             OpenFileId fileId = currentThread -> space -> fileOpen(arg);
             machine -> WriteRegister(2, fileId);
             
          case SC_Close:
              currentThread -> space -> fileClose(machine->ReadRegister(4));
              
          case SC_Read:
              int intoBuf = machine -> ReadRegister(4);
              int size = machine -> ReadRegister(5);
              int fileId = machine->ReadRegister(6);
              
              OpenFile* readFile = currentThread -> space -> readWrite(fileId);
              
              if (readFile == nullptr)
                  machine -> WriteRegister(2, -1);
              
              else
                  machine -> WriteRegister(2, readFile -> Read(intoBuf, size));
              
          case 
              
              
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
    result = new(std::nothrow) char[size];
    
    for (int i = 0; i < size; i++){
        if ((result[i] = machine->mainMemory[location++]) == '\0')
            break;
    }
    
    result[size] = '\0';
    
}


