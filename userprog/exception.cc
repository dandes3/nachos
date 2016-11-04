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

void ReadArg(char* result, int size, bool write);
void IncrementPc();
void CopyThread(int prevThreadPtr);
int ConvertAddr (int virtualAddress);
void killThread(int exitVal);
void execThread(int garbage);

void
ExceptionHandler(ExceptionType which)
{


#ifdef CHANGED
    int type = machine->ReadRegister(2);
    int size, intoBuf, readBytes, fileType, physIntoBuf, cid, str, argc, physAddr, argAddr,  *seg;
    OpenFileId fileId;
    OpenFile* readFile, *writeFile;
    Thread* newThread;
 
    char* arg, *readContent, stdinChar, **execArgs;
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
            ReadArg(arg, 127, false); //put name of file in arg, max 127 chars, truncated if over 
    
            fileSystem -> Create(arg, -1); 
            
            IncrementPc(); //Increment program counter to next instruction
            
            break;
            
        case SC_Open:
            
            DEBUG('a', "Open entered\n");

            arg = new(std::nothrow) char[128];
            ReadArg(arg, 127, false);

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
            forkExec -> Acquire();
            DEBUG('p', "Write entered\n");
            //printf("Write entered by %s\n", currentThread -> getName());
            //printf("At top of write, PrevPC: %d, PC: %d, NextPC: %d\n", machine -> ReadRegister(PrevPCReg), machine -> ReadRegister(PCReg), machine -> ReadRegister(NextPCReg));

            size = machine -> ReadRegister(5); //Number of bytes to be written
            fileId = machine->ReadRegister(6); //File descriptor of file to be written
            
            DEBUG('p', "size is: %d\n", size); 
            //Pull content to be written into local memory
            arg = new(std::nothrow) char[size];
            DEBUG('p', "arg allocated\n");
            ReadArg(arg, size, true);
           
	        DEBUG('p', "After ReadArg\n");
 
            writeFile = currentThread -> space -> readWrite(fileId); //OpenFile object corresponding to file descriptor
            fileType = currentThread -> space -> isConsoleFile(writeFile); //Int describing if OpenFile object is stdin, stdout or neither

            if (fileType  == 1){ //stdout
               stdOut -> Acquire();
                for (int i = 0; i < size; i++)
                    sConsole -> PutChar(arg[i]); //Put each char using SynchConsole
                    
               stdOut -> Release();
            } 

            else if (writeFile != NULL && fileType != 0) {//File descriptor was valid and not stdin
                writeFile -> offsetLock -> Acquire();
                writeFile -> Write(arg, size);
                writeFile -> offsetLock -> Release();
            }
            
            
            IncrementPc();
            //fprintf(stderr, "Exiting write: My page table addr %x, machine page table addr %x\n", currentThread -> space -> pageTable, machine -> pageTable);
          //  currentThread -> space -> RestoreState();
     //       printf("At bottom of write, PrevPC: %d, PC: %d, NextPC: %d\n", machine -> ReadRegister(PrevPCReg), machine -> ReadRegister(PCReg), machine -> ReadRegister(NextPCReg));
       //     printf("%s's personal PC %d\n", currentThread -> getName(), currentThread -> userRegisters[PCReg]);
           forkExec -> Release();
            break;
            
        case SC_Fork:
            forkExec -> Acquire();
            
            fprintf(stderr, "Fork entered by %s\n", currentThread -> getName());
            //fprintf(stderr, "My page table addr %x, machine page table addr %x\n", currentThread -> space -> pageTable, machine -> pageTable);
            //fprintf(stderr, "PC at top of fork: %d\n", machine -> ReadRegister(PCReg));
            
            spaceIdSem -> P();
            cid = spaceId ++;
           
            spaceIdSem -> V();
            //MEGALOCK -> Acquire();
             bzero(childName, 1024);
            snprintf(childName, 1024, "child%d", cid);
            newThread = new Thread(childName); //Find a way to get childId into child thread addrSpace
            newThread -> space = new (std::nothrow) AddrSpace(currentThread -> space);
            //MEGALOCK -> Release();
            
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
            newThread -> SaveUserState();
 
            for (int i = 0; i < NumTotalRegs; i++)
                newThread -> userRegisters[i] = machine -> ReadRegister(i);
            
            newThread -> Fork(CopyThread, 0); 
            
            //printf("JoinList after fork\n");
            //joinList -> print();

            forkSem -> P();
            //currentThread -> space ->  RestoreState();
            currentThread -> RestoreUserState();
            machine -> WriteRegister(2, cid);
            forkExec -> Release();
            //fprintf(stderr, "%s off lock semaphore\n", currentThread -> getName());
           // fprintf(stderr, "PC after fork: %d\n", machine -> ReadRegister(PCReg));
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
            forkExec -> Acquire();
            //fprintf(stderr, "In Exit\n");
            killThread(machine -> ReadRegister(4));
            break;
            
        case SC_Exec:
            forkExec -> Acquire();
            
            //fprintf(stderr, "In exec\n");
            arg = new(std::nothrow) char[128];
            ReadArg(arg, 127, false); 
            
           // MEGALOCK -> Acquire();
            bzero(childName, 1024);
            snprintf(childName, 1024, "%s exec", currentThread -> name);
            newThread = new(std::nothrow) Thread(childName);
          //  MEGALOCK -> Release();
            
            //printf("name created\n");
            newThread -> space = new(std::nothrow) AddrSpace(fileSystem -> Open(arg));
            if (newThread -> space -> failed){
                machine -> WriteRegister(2, -1);
                IncrementPc();
                forkExec -> Release();
                break;
            }
                
            printf("space created\n");
            newThread -> space -> parentThreadPtr = currentThread -> space -> parentThreadPtr;
            newThread -> space -> mySpaceId = currentThread -> space -> mySpaceId;
            newThread -> space -> stdIn = currentThread -> space -> stdIn;
            newThread -> space -> stdOut = currentThread -> space -> stdOut;
            newThread -> space -> fileName = arg;
            
            
            argAddr = machine -> ReadRegister(5);
            if (argAddr != 0){
                execArgs = new(std::nothrow) char* [128];
                physAddr = ConvertAddr(argAddr);
                //printf("Phys addr is:%d, at that address is %c, as int is %d\n", physAddr, machine -> mainMemory[physAddr], machine -> mainMemory[physAddr] );
                //printf("At that address is: %d\n", machine -> mainMemory[ConvertAddr(machine -> mainMemory[physAddr])]);

                argc = 0;

            // printf("Above while\n");
                while ((str = *(unsigned int *) &machine -> mainMemory[physAddr]) != 0){ 
                //  printf("Str equals: %d\n", str);
                    int curChar = ConvertAddr((int)str);
                    int count = 0;
                    //printf("IN while, curChar equals: %d, and the char is: %c\n", curChar, machine -> mainMemory[curChar]);
                    
                    
                    while (machine -> mainMemory[curChar] != '\0'){
                        str ++;
                        curChar = ConvertAddr((int) str);
                        count ++;
                    }
                    
                    execArgs[argc] = new(std::nothrow) char[count + 1]; //TODO: Check not over 128 args
                    
                    
                    str = str - count;
                    curChar = ConvertAddr((int)str);
                    count = 0;
                    while (machine -> mainMemory[curChar] != '\0'){
                        execArgs[argc][count] = machine -> mainMemory[curChar];
                        str ++;
                        curChar = ConvertAddr((int) str);
                        count ++;
                    }
                    
                    execArgs[argc][count] = '\0';
                    
                    argc ++;
                    
                    argAddr += 4;
                    physAddr = ConvertAddr(argAddr);
                //  printf("Count: %d\n", count);
               }
                
                execArgs[argc] = NULL;
            }
            
            else
                execArgs = NULL;
            
            for (int i = 0; i < 20; i++)
                newThread -> space -> fileVector[i] = currentThread -> space -> fileVector[i];
            
            newThread -> Fork(execThread, (int) execArgs);
           // forkExec -> Release();
            
            killThread(-12);
            ASSERT(false); //Should never be reached
            break;
            
            case SC_Dup:
                fprintf(stderr, "in dup\n");
                fprintf(stderr, "arg to dup is %d\n", machine -> ReadRegister(4));
                machine -> WriteRegister(2, currentThread -> space -> dupFd(machine -> ReadRegister(4)));
                fprintf(stderr, "past dup stuff\n");
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
    case NoException:

          break;
    /*
    case ReadOnlyException:
          break;
        
    case BusErrorException:
          break;
					    // invalid physical address
    case AddressErrorException: 

          break;
					    // was beyond the end of the
					    // address space
    case OverflowException:
        fprintf(stderr, "overflow\n");// Integer overflow in add or sub.
          break;
    case IllegalInstrException:
        fprintf(stderr, "IllegalInstr\n");// Unimplemented or reserved instr.
        seg = 0;
        printf("%d", *seg);
		break;
    case NumExceptionTypes:
        fprintf(stderr, "NumExceptionTypes\n");
        break;*/
    default:
        forkExec->Acquire(); //TODO: determine this
        killThread(2);
    }
    
    
}

#ifdef CHANGED
/*
* Pulls "size" number of characters from register 4, and puts them into
* the local buffer "result".
*/
void ReadArg(char* result, int size, bool write){ //Size refers to last index of array
    
    int location, physAddr;
    location = machine->ReadRegister(4);

    for (int i = 0; i < size; i++){
        physAddr = ConvertAddr(location);
        //fprintf(stderr, "PhysAddr: %d, char from mem: %c\n", physAddr, machine -> mainMemory[physAddr]);
        if ((result[i] = machine->mainMemory[physAddr]) == '\0')
            break;
        location++;
    }
    
    if (!write)
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

int ConvertToVirtual (int physicalAddress){
    int physPage = physicalAddress / PageSize;
    int offset = physicalAddress % PageSize;
    
    for (int i = 0; i < currentThread -> space -> numPages; i++){
        if (currentThread -> space -> pageTable[i].physicalPage == physPage)
            return currentThread -> space -> pageTable[i].physicalPage * PageSize + offset;
    }
    
    return -1;
}


void CopyThread(int garbage){
    fprintf(stderr, "%s In CopyThread\n", currentThread -> name);
    forkSem -> V();
     
    currentThread -> space -> RestoreState();
    
     //fprintf(stderr, "In copythread, my pageTable %x, machine page table %x\n", currentThread -> space -> pageTable, machine -> pageTable);
    //printf("Past RestoreState\n");
    for (int i = 0; i < NumTotalRegs; i++)
       //printf("Machine : %d, forked thread: %d\n", machine -> ReadRegister(i), currentThread -> userRegisters[i]);
	    machine -> WriteRegister(i, currentThread -> userRegisters[i]);

    machine -> Run();
    fprintf(stderr, "Past machine run\n");
}
#endif

void execThread(int argsInt){
    currentThread -> space -> RestoreState();
    currentThread -> space -> InitRegisters();
    
    if (argsInt != 0){//TODO: If no args, file name should still be arg 0
        char** args = (char**) argsInt;
        int argc = 0;
        
        
    // printf("execThread entered\n");
        while (args[argc] != NULL){
            //fprintf(stderr, "Argument %d: %s\n", argc, args[argc]);
            argc ++;
        }
        
        int* argAddrs = new(std::nothrow) int[argc];
        
    // printf("argc created\n");
        
    // fprintf(stderr, "Sp equals %d\n", machine -> ReadRegister(StackReg));
        
        
        int sp = machine -> ReadRegister(StackReg);
        
        int len;

/*
        sp -= len;
        for (int i = 0; i < len; i++)
            machine -> mainMemory[ConvertAddr(sp + i)] =  currentThread -> space -> fileName[i];
        
        argAddrs[0] = sp;
        //fprintf(stderr, "Filename put in mem\n");
  */      
        
        for (int i = 0; i < argc; i ++){
            //fprintf(stderr, "Argumet: %s, Arglen: %d, Sp physaddr: %d
            len = strlen(args[i]) + 1;
            sp -= len;
            for (int j = 0; j < len; j++)
                machine -> mainMemory[ConvertAddr(sp + j)] = args[i][j];   
            
            argAddrs[i] = sp;
        }
        
        //fprintf(stderr, "Args put in mem\n");
        
        sp = sp & ~3;
        
        sp -= sizeof(int) * (argc);
        
        for (int i = 0; i < argc; i ++)
            *(unsigned int *) &machine -> mainMemory[ConvertAddr(sp + i*4)] = WordToMachine((unsigned int) argAddrs[i]);
        
        machine -> WriteRegister(4, argc);
        machine -> WriteRegister(5, sp);
        
        machine -> WriteRegister(StackReg,sp - 8);
    }
    else
        fprintf(stderr, "skipped exec args\n");
    
    currentThread -> SaveUserState();
    
   // fprintf(stderr, "about run\n");
    machine -> Run();
    
}

void killThread(int exitVal){
    
    AddrSpace* space = currentThread -> space;
    
    
     
    
    //fprintf(stderr, "PhysicalPages cleared by %s:", currentThread -> name);
    
    for (int i = 0; i < space -> numPages; i ++){
        bitLock -> Acquire();
        //fprintf(stderr, "%d ", space -> pageTable[i].physicalPage);
        memMap -> Clear(space -> pageTable[i].physicalPage);   
        bitLock -> Release();
    }
   // fprintf(stderr, "\n");
    
   
    if (exitVal != -12){
        joinSem -> P();
        JoinNode* joinNode = joinList -> getNode((Thread*) space -> parentThreadPtr, space -> mySpaceId);
        joinSem -> V();
        
        if (joinNode != NULL){
            joinNode -> exitVal = exitVal;
           // printf("JoinList after exit\n");
           //joinList -> print();
            joinNode -> permission -> V();
        }
    }
    
    //fprintf(stderr, "Exiting killThread\n");
    forkExec -> Release();
    currentThread -> Finish();    
}
