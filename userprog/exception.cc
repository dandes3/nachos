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
void IncrementPc();

void
ExceptionHandler(ExceptionType which)
{
    
    int type = machine->ReadRegister(2);
    int size, intoBuf, readBytes, fileType;
    OpenFileId fileId;
    OpenFile* readFile, *writeFile;
    
    char* arg, *readContent;
    
    switch (which) {
      case SyscallException:
	    switch (type) {
          case SC_Halt:
             DEBUG('a', "Shutdown, initiated by user program.\n");
             interrupt->Halt();
             break;

#ifdef CHANGED       
          case SC_Create:
             DEBUG('a', "Create entered\n");

             arg = new(std::nothrow) char[128];
             ReadArg(arg, 127);
       
             fileSystem -> Create(arg, -1);
             
             IncrementPc();  
             break;
             
          case SC_Open:
             DEBUG('a', "Open entered\n");

             arg = new(std::nothrow) char[128];
             ReadArg(arg, 127);

             fileId = currentThread -> space -> fileOpen(arg);
             machine -> WriteRegister(2, fileId);

             IncrementPc();
             break;
             
          case SC_Close:
              DEBUG('a', "Close entered\n");

              currentThread -> space -> fileClose(machine->ReadRegister(4));
             
              IncrementPc();
              break;
              
          case SC_Read:
              DEBUG('a', "Read entered\n");

              intoBuf =  machine -> ReadRegister(4);
              size = machine -> ReadRegister(5);
              fileId = machine->ReadRegister(6);
              readContent = new(std::nothrow) char[size];
 
              readFile = currentThread -> space -> readWrite(fileId);
              fileType = currentThread -> space -> isConsoleFile(readFile);

              if (fileType  == 1); //Needs to be implemented, stdOut
              else if (readFile == 0); //Needs to be implemented, stdIn
 
              else if (readFile == NULL)
                  machine -> WriteRegister(2, -1);
              
              else{
                  readBytes  =  readFile -> Read(readContent, size); 
                  machine -> WriteRegister(2, readBytes);
      
                  for (int i = 0; i < size; i++){
                     machine -> mainMemory[intoBuf] = readContent[i];
                     intoBuf++;
                  }
              }

              IncrementPc();
              break;
              
          case SC_Write:
              DEBUG('a', "Write entered\n");

              size = machine -> ReadRegister(5);
              fileId = machine->ReadRegister(6);
    
              arg = new(std::nothrow) char[size];
              ReadArg(arg, size);
              
              writeFile = currentThread -> space -> readWrite(fileId);
              fileType = currentThread -> space -> isConsoleFile(writeFile);

              if (fileType  == 1); //Needs to be implementd, stdOut 
              else if (fileType == 0); //Needs to be implemented, stdIn

              else if (writeFile != NULL)
                  writeFile -> Write(arg, size);
              
              IncrementPc();
              break;
#endif              
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

#ifdef CHANGED
void ReadArg(char* result, int size){ //Size refers to last index of array
    
    int location;
    location = machine->ReadRegister(4);

    for (int i = 0; i < size; i++){
        if ((result[i] = machine->mainMemory[location++]) == '\0')
            break;
    }
    
    result[size] = '\0';
    
}

void IncrementPc(){

    int tmp;

    tmp = machine -> ReadRegister(PCReg);
    machine -> WriteRegister(PrevPCReg, tmp);
    tmp = machine -> ReadRegister(NextPCReg);
    machine -> WriteRegister(PCReg, tmp);
    tmp += 4;
    machine -> WriteRegister(NextPCReg, tmp);
}

#endif
