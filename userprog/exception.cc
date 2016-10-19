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
//IMPORTANT: All code written assumes that ExceptionHandler cannot be executed by two threads concurrently 

#ifdef CHANGED
    int type = machine->ReadRegister(2);
    int size, intoBuf, readBytes, fileType;
    OpenFileId fileId;
    OpenFile* readFile, *writeFile;
 
    char* arg, *readContent, stdinChar;
#endif

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
            ReadArg(arg, 127); //put name of file in arg, max 127 chars, truncated if over 
    
            fileSystem -> Create(arg, -1); 
            
            IncrementPc(); //Increment program counter to next instruction
            break;
            
        case SC_Open:
            DEBUG('a', "Open entered\n");

            arg = new(std::nothrow) char[128];
            ReadArg(arg, 127);

            fileId = currentThread -> space -> fileOpen(arg); //Put OpenFile object in thread file vector, get file descriptor
            machine -> WriteRegister(2, fileId); //Return file descriptor

            IncrementPc();
            break;
            
        case SC_Close:
            DEBUG('a', "Close entered\n");

            currentThread -> space -> fileClose(machine->ReadRegister(4)); //Remove file from file vector, delete OpenFile object
            
            IncrementPc();
            break;
            
        case SC_Read:
            DEBUG('a', "Read entered\n");

            intoBuf =  machine -> ReadRegister(4); //Return buffer
            size = machine -> ReadRegister(5); //Number of bytes requested
            fileId = machine -> ReadRegister(6); //File descriptor from which to read

            readContent = new(std::nothrow) char[size + 1]; //Local buffer to pull data from file
            bzero(readContent, size + 1); //If fewer than size bytes are read, intoBuf will be filled with null bytes

            readFile = currentThread -> space -> readWrite(fileId); //OpenFile object associated with file descriptor
            fileType = currentThread -> space -> isConsoleFile(readFile); //Returns an int which tells if the OpenFile object is stdin or stdout or neither
 
            if (fileType  == 1 || readFile == NULL){  //Requested read on stdin, or a file descriptor that doesn't correspond to a file
                DEBUG('p', "Error in Read\n");
                machine -> WriteRegister(2, -1); //Return -1
                IncrementPc();
                break;
            }

            else if (fileType  == 0){ //Read from stdin
                readBytes = 0;
                for (int i = 0; i < size; i++){ //Read size chars, or until EOF is reached
                    sConsole -> GetChar(&stdinChar); 
                    if (stdinChar == EOF)
                        break;
                    else{
                        readContent[i] = stdinChar;
                        readBytes ++;
                   }
                }
            }
                        
            else //Read from a file
                readBytes  =  readFile -> Read(readContent, size); //Read into local buffer, returns number of bytes read

            //Only reached if a read is actually attempted
            machine -> WriteRegister(2, readBytes); //Write number of bytes read into return register
            for (int i = 0; i < size; i++){ //Copy local buffer into main memory at intoBuf address
                    machine -> mainMemory[intoBuf] = readContent[i];
                    intoBuf++;
            }

            IncrementPc();
            break;
            
        case SC_Write:
            DEBUG('a', "Write entered\n");

            size = machine -> ReadRegister(5); //Number of bytes to be written
            fileId = machine->ReadRegister(6); //File descriptor of file to be written
      
            //Pull content to be written into local memory
            arg = new(std::nothrow) char[size];
            ReadArg(arg, size);
            
            writeFile = currentThread -> space -> readWrite(fileId); //OpenFile object corresponding to file descriptor
            fileType = currentThread -> space -> isConsoleFile(writeFile); //Int describing if OpenFile object is stdin, stdout or neither

            if (fileType  == 1){ //stdout
                for (int i = 0; i < size; i++)
                    sConsole -> PutChar(arg[i]); //Put each char using SynchConsole
            } 

            else if (writeFile != NULL && fileType != 0) //File descriptor was valid and not stdin
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
/*
* Pulls "size" number of characters from register 4, and puts them into
* the local buffer "result".
*/
void ReadArg(char* result, int size){ //Size refers to last index of array
    
    int location;
    location = machine->ReadRegister(4);

    for (int i = 0; i < size; i++){
        if ((result[i] = machine->mainMemory[location++]) == '\0')
            break;
    }
    
    result[size] = '\0';
}

/*
* Increments the program counter to the next instruction after the syscall.
*/
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
