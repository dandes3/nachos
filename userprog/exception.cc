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
void CopyThread(int prevThreadPtr);
int ConvertAddr (int virtualAddress);
void killThread(int exitVal);
void execThread(int garbage);

void
ExceptionHandler(ExceptionType which)
{
//IMPORTANT: All code written assumes that ExceptionHandler cannot be executed by two threads concurrently 

#ifdef CHANGED
    int type = machine->ReadRegister(2);
    int size, intoBuf, readBytes, fileType, physIntoBuf, cid;
    OpenFileId fileId;
    OpenFile* readFile, *writeFile;
    Thread* newThread;
 
    char* arg, *readContent, stdinChar;
    char childName[1024];
    JoinNode* joinNode;
#endif
    
    switch (which) {
    case SyscallException:
        //fprintf(stderr, "in syscall\n");
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

            intoBuf = machine -> ReadRegister(4); //Return buffer
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
                        
            else{ //Read from a file
                readFile -> offsetLock -> Acquire();
                readBytes  =  readFile -> Read(readContent, size); //Read into local buffer, returns number of bytes read
                readFile -> offsetLock -> Release();
            }
            //Only reached if a read is actually attempted
            machine -> WriteRegister(2, readBytes); //Write number of bytes read into return register
            for (int i = 0; i < size; i++){ //Copy local buffer into main memory at intoBuf address
                    physIntoBuf = ConvertAddr(intoBuf);
                    machine -> mainMemory[physIntoBuf] = readContent[i];
                    intoBuf++;
            }

            IncrementPc();
            break;
            
        case SC_Write:
            DEBUG('p', "Write entered\n");
            //printf("Write entered by %s\n", currentThread -> getName());
         //   printf("At top of write, PrevPC: %d, PC: %d, NextPC: %d\n", machine -> ReadRegister(PrevPCReg), machine -> ReadRegister(PCReg), machine -> ReadRegister(NextPCReg));

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

            else if (writeFile != NULL && fileType != 0) {//File descriptor was valid and not stdin
                writeFile -> offsetLock -> Acquire();
                writeFile -> Write(arg, size);
                writeFile -> offsetLock -> Release();
            }
            
            
            IncrementPc();
          //  currentThread -> space -> RestoreState();
     //       printf("At bottom of write, PrevPC: %d, PC: %d, NextPC: %d\n", machine -> ReadRegister(PrevPCReg), machine -> ReadRegister(PCReg), machine -> ReadRegister(NextPCReg));
       //     printf("%s's personal PC %d\n", currentThread -> getName(), currentThread -> userRegisters[PCReg]);
            break;
            
        case SC_Fork:

            //fprintf(stderr, "Fork entered by %s\n", currentThread -> getName());
            //fprintf(stderr, "My page table addr %x, machine page table addr %x\n", currentThread -> space -> pageTable, machine -> pageTable);
            //fprintf(stderr, "PC at top of fork: %d\n", machine -> ReadRegister(PCReg));
            
            spaceIdSem -> P();
            cid = spaceId ++;
            bzero(childName, 1024);
            snprintf(childName, 1024, "child%d", cid);
            spaceIdSem -> V();
            
            newThread = new(std::nothrow) Thread(childName); //Find a way to get childId into child thread addrSpace
            newThread -> space = new (std::nothrow) AddrSpace(currentThread -> space);
            
            if (newThread -> space -> failed){
                machine -> WriteRegister(2, -1);
                IncrementPc();
                break;
            }
            
            newThread -> space -> parentThreadPtr = (int) currentThread;
            newThread -> space -> mySpaceId = cid;
            
            joinSem -> P();
            joinList -> addNode(currentThread, cid);
            joinSem -> V();
            
            machine -> WriteRegister(2, 0); //Put semaphores around spaceID
            IncrementPc();
            currentThread -> SaveUserState();
 
            for (int i = 0; i < NumTotalRegs; i++)
                newThread -> userRegisters[i] = machine -> ReadRegister(i);
            
            newThread -> Fork(CopyThread, 0); 
            
            //printf("JoinList after fork\n");
            //joinList -> print();

            forkSem -> P();
            currentThread -> space ->  RestoreState();
            currentThread -> RestoreUserState();
            machine -> WriteRegister(2, cid);
            //fprintf(stderr, "%s off lock semaphore\n", currentThread -> getName());
            //fprintf(stderr, "PC after fork: %d\n", machine -> ReadRegister(PCReg));
            //fprintf(stderr, "Leaving fork: My page table addr %x, machine page table addr %x\n", currentThread -> space -> pageTable, machine -> pageTable);
            break;
            
        case SC_Join:
            //fprintf(stderr, "In Join\n");
            cid = machine -> ReadRegister(4);
           
            joinSem -> P();
            joinNode = joinList -> getNode(currentThread, cid);
            joinSem -> V();
            
            if (joinNode == NULL)
                machine -> WriteRegister(2, -1);
            
            else{
                joinNode -> permission -> P();
                machine -> WriteRegister(2, joinNode -> exitVal);
                
                joinSem -> P();
                //fprintf(stderr, "About to delete node\n");
                joinList -> deleteNode(joinNode);
                joinSem -> V();
            }
            
            //fprintf(stderr, "Exiting Join\n");
            
            //printf("JoinList after join\n");
            //joinList -> print();

            IncrementPc();
            break;
            
        case SC_Exit:
            //fprintf(stderr, "In Exit\n");
            killThread(machine -> ReadRegister(4));
            break;
            
        case SC_Exec:
            //fprintf(stderr, "In exec\n");
            arg = new(std::nothrow) char[128];
            ReadArg(arg, 127);
            
            bzero(childName, 1024);
            snprintf(childName, 1024, "%s exec", currentThread -> name);
            newThread = new(std::nothrow) Thread(childName); 
            //printf("name created\n");
            newThread -> space = new(std::nothrow) AddrSpace(fileSystem -> Open(arg));
            if (newThread -> space -> failed){
                machine -> WriteRegister(2, -1);
                IncrementPc();
                break;
            }
                
            printf("space created\n");
            newThread -> space -> parentThreadPtr = currentThread -> space -> parentThreadPtr;
            newThread -> space -> mySpaceId = currentThread -> space -> mySpaceId;
            newThread -> space -> stdIn = currentThread -> space -> stdIn;
            newThread -> space -> stdOut = currentThread -> space -> stdOut;
            
            for (int i = 0; i < 20; i++)
                newThread -> space -> fileVector[i] = currentThread -> space -> fileVector[i];
            
            newThread -> Fork(execThread, 0);
            killThread(-12);
            ASSERT(false); //Should never be reached

            
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
 
    case NoException:
          //fprintf(stderr, "NoException\n");
          break;
        
    case ReadOnlyException:
        fprintf(stderr, "ReadOnly\n");
          break;
        
    case BusErrorException:
        fprintf(stderr, "BusError\n");// Translation resulted in an 
          break;
					    // invalid physical address
    case AddressErrorException: // Unaligned reference or one that
          fprintf(stderr, "AddressError\n");
          break;
					    // was beyond the end of the
					    // address space
    case OverflowException:
        fprintf(stderr, "overflow\n");// Integer overflow in add or sub.
          break;
    case IllegalInstrException:
        fprintf(stderr, "IllegalInstr\n");// Unimplemented or reserved instr.
		break;
    case NumExceptionTypes:
        fprintf(stderr, "NumExceptionTypes\n");
        break;
    default: 
        fprintf(stderr, "defaulting\n");
    }
    
    
}

#ifdef CHANGED
/*
* Pulls "size" number of characters from register 4, and puts them into
* the local buffer "result".
*/
void ReadArg(char* result, int size){ //Size refers to last index of array
    
    int location, physAddr;
    location = machine->ReadRegister(4);

    for (int i = 0; i < size; i++){
        physAddr = ConvertAddr(location);
        //fprintf(stderr, "PhysAddr: %d, char from mem: %c\n", physAddr, machine -> mainMemory[physAddr]);
        if ((result[i] = machine->mainMemory[physAddr]) == '\0')
            break;
        location++;
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

int ConvertAddr (int virtualAddress){
    int virtPage = virtualAddress / PageSize;
    int offset = virtualAddress % PageSize;
    
    return ((currentThread -> space -> pageTable[virtPage].physicalPage) * PageSize) + offset;
}
/*
void CopyThread(int prevThreadPtr){
    fprintf(stderr, "In CopyThread\n");
    Thread* prevThread = (Thread*) prevThreadPtr;
   
    //prevThread -> RestoreUserState();
    fprintf(stderr, "After AddrSpace is copied\n");
    currentThread -> RestoreUserState();
    machine -> WriteRegister(2, 0);
    fprintf(stderr, "PC in machine: %d, PC in child: %d\n", machine -> ReadRegister(PCReg), currentThread -> userRegisters[PCReg]);
    
    fprintf(stderr, "Before run\n");
    forkSem -> V();
    machine -> Run();
}
*/

void CopyThread(int garbage){
    //printf("In CopyThread\n");
    forkSem -> V();
     
    currentThread -> space -> RestoreState();
    //printf("Past RestoreState\n");
    for (int i = 0; i < NumTotalRegs; i++)
       //printf("Machine : %d, forked thread: %d\n", machine -> ReadRegister(i), currentThread -> userRegisters[i]);
	   machine -> WriteRegister(i, currentThread -> userRegisters[i]);

    machine -> Run();
}
#endif

void execThread(int garbage){
    currentThread -> space -> RestoreState();
    currentThread -> space -> InitRegisters();
    
    machine -> Run();
    
}

void killThread(int exitVal){
    
    AddrSpace* space = currentThread -> space;
    
    for (int i = 0; i < space -> numPages; i ++)
        memMap -> Clear(space -> pageTable[i].physicalPage);   
    
    if (exitVal != -12){
        joinSem -> P();
        JoinNode* joinNode = joinList -> getNode((Thread*) space -> parentThreadPtr, space -> mySpaceId);
        joinSem -> V();
        
        if (joinNode != NULL){
            joinNode -> exitVal = exitVal;
            //printf("JoinList after exit\n");
            // joinList -> print();
            joinNode -> permission -> V();
        }
    }
    
    //fprintf(stderr, "Exiting killThread\n");
    
    currentThread -> Finish();    
}
