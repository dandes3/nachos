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
void KillThread(int exitVal);
void ExecThread(int garbage);
void CopyExecArgs(char** execArgs, int argAddr);
int pageToRemove();
void faultPage(int, bool);
void bringIntoMemory(int);
void CheckPointThread(int garbage);

void
ExceptionHandler(ExceptionType which)
{


#ifdef CHANGED
    int type = machine->ReadRegister(2);
    int size, intoBuf, readBytes, fileType, physIntoBuf, cid, argAddr, currentPage, checkLen;
    OpenFileId fileId;
    OpenFile* readFile, *writeFile, *execFile, *checkFile;
    Thread* newThread;
 
    char* arg, *readContent, stdinChar, **execArgs, *checkValue, *regValue;
    char childName[1024];
    JoinNode* joinNode;

    DEBUG('a', "Exception.cc entered\n");
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
            ReadArg(arg, 127, false); //Put name of file in arg, max 127 chars, truncated if over 
    
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
            
            currentPage = intoBuf / PageSize;
            bringIntoMemory(intoBuf); 
            
            for (int i = 0; i < size; i++){ //Copy local buffer into main memory at intoBuf address
                if (intoBuf / PageSize != currentPage){
                    vmInfoLock -> Acquire();
                    faultInfo[currentThread -> space -> pageTable[(intoBuf - 1) / PageSize].physicalPage] -> locked = false;
                    vmInfoLock -> Release();
                    
                    currentPage = intoBuf / PageSize;
                    bringIntoMemory(intoBuf);
                }
                
                physIntoBuf = ConvertAddr(intoBuf);
                machine -> mainMemory[physIntoBuf] = readContent[i];
                intoBuf++;
            }
            
            vmInfoLock -> Acquire();
            faultInfo[currentThread -> space -> pageTable[intoBuf / PageSize].physicalPage] -> locked = false;
            vmInfoLock -> Release();   

            IncrementPc();
            break;
            
        case SC_Write:
            DEBUG('w', "Write entered\n");
 
            size = machine -> ReadRegister(5); //Number of bytes to be written
            fileId = machine->ReadRegister(6); //File descriptor of file to be written
            
            //Pull content to be written into local memory
            arg = new(std::nothrow) char[size];  
            ReadArg(arg, size, true);
            
            writeFile = currentThread -> space -> readWrite(fileId); //OpenFile object corresponding to file descriptor
            fileType = currentThread -> space -> isConsoleFile(writeFile); //Int describing if OpenFile object is stdin, stdout or neither

            DEBUG('a', "File type: %d\n", fileType);
            
            if (fileType  == 1){ //stdout
               DEBUG('a', "Writing to stdout\n");
               stdOut -> Acquire(); //Atomic writes to console
                for (int i = 0; i < size; i++)
                    sConsole -> PutChar(arg[i]); //Put each char using SynchConsole
                    
               stdOut -> Release();
            } 

            else if (writeFile != NULL && fileType != 0) {//File descriptor was valid and not stdin
                atomicWrite -> Acquire(); //Atomic writes across different openfile 
            
                writeFile -> Write(arg, size);
                
                atomicWrite -> Release();
            }
            
            DEBUG('a', "Write exited\n");
            
            IncrementPc();
            break;
            
        case SC_Fork:
            forkExec -> Acquire(); //Exec and Fork are mutually exclusive
            
            spaceIdSem -> P();
           
            cid = spaceId ++; //Process id for a child process, locked to ensure a unique id per process
           
            spaceIdSem -> V();

            bzero(childName, 1024);
            snprintf(childName, 1024, "child%d", cid); //Create name for thread based on its process id
            newThread = new(std::nothrow) Thread(childName); 
            
            newThread -> space = new(std::nothrow) AddrSpace(currentThread -> space); //Copies parent's addr space, work done within addrspace.cc
            
            if (newThread -> space -> failed){ //If the fork fails, the addrspace constructor sets the failed flag, allows constructor to "return a value"
                machine -> WriteRegister(2, -1);
                IncrementPc();
                break;
            }
            
            newThread -> space -> parentThreadPtr = (int) currentThread; //Used in join to find correct node in joinList
            newThread -> space -> mySpaceId = cid; //See above comment
            
            joinSem -> P();
            joinList -> addNode(currentThread, cid); //Adds a node to the global joinList
            joinSem -> V();
            
            machine -> WriteRegister(2, 0); //Return value for child in Fork
            IncrementPc();//Effectively Increments PC for child and parent
            
            newThread -> SaveUserState(); //Save prepped registers
 
            newThread -> Fork(CopyThread, 0); //Child will complete forking procedure in CopyThread

            forkSem -> P(); //Wait until child is done with CopyThread to execute
            currentThread -> RestoreUserState(); //Ensure that registers return to state before sleeping
            machine -> WriteRegister(2, cid); //Correct return value for parent
            forkExec -> Release();

            break;
            
        case SC_Join:
            cid = machine -> ReadRegister(4);
           
            joinSem -> P();
            joinNode = joinList -> getNode(currentThread, cid); //Get node for the child for whom you are waiting
            joinSem -> V();
            
            if (joinNode == NULL)
                machine -> WriteRegister(2, -1); //Process id is not valid
            
            else{
                joinNode -> permission -> P(); //Sleep here until child wakes you up
                
                machine -> WriteRegister(2, joinNode -> exitVal); //By the time the parent wakes up, the exit value will have been
                                                                  //implanted in the joinNode
                joinSem -> P();
                joinList -> deleteNode(joinNode); //Delete node from global join list, the child has exited and already been joined
                joinSem -> V();
            }

            IncrementPc();
            break;
            
        case SC_Exit:
            KillThread(machine -> ReadRegister(4)); //Release memory, update joinlist and finish thread
            
            break;
            
        case SC_Exec:
            forkExec -> Acquire(); //Exec and Fork are mutually exclusive
            
            arg = new(std::nothrow) char[128]; //Name of executable
            ReadArg(arg, 127, false); 

            bzero(childName, 1024);
            snprintf(childName, 1024, "%s exec", currentThread -> name); //Create exec name based on name of execer
            newThread = new(std::nothrow) Thread(childName);

            execFile = fileSystem -> Open(arg); //Assuming the currency pointer will be moved in addrspace creation if checkpoint
            newThread -> space = new(std::nothrow) AddrSpace(execFile); //Construct an addrspace just as if the main thread were being initialized
            
            if (newThread -> space -> failed){ //File doesn't exist, isn't an noff, not enough memory, etc.
                machine -> WriteRegister(2, -1); //Exec failed
                IncrementPc();
                forkExec -> Release();
                break;
            }
                
            //Update the newly execed thread so that it can exit and join as if it were the thread execing it
            newThread -> space -> parentThreadPtr = currentThread -> space -> parentThreadPtr;
            newThread -> space -> mySpaceId = currentThread -> space -> mySpaceId;
            newThread -> space -> fileName = arg;
            
            if (!newThread -> space -> checkpoint){
                argAddr = machine -> ReadRegister(5); //Virtual address of exec argument array
                execArgs = new(std::nothrow) char* [128];
                DEBUG('v', "Argaddr straight from register is %d\n", argAddr);
                
                CopyExecArgs(execArgs, argAddr); //Copy exec args from user memory to the kernel
                
                //Copy parent's file vector
                for (int i = 0; i < 20; i++)
                    newThread -> space -> fileVector[i] = currentThread -> space -> fileVector[i];
                
                newThread -> space -> stdIn = currentThread -> space -> stdIn;
                newThread -> space -> stdOut = currentThread -> space -> stdOut;
                
                newThread -> Fork(ExecThread, (int) execArgs); //Forked child will complete exec prep in ExecThread
            }
            
            else{
                DEBUG('c', "Current offset %d\n", execFile -> currentOffset);
                regValue = new (std::nothrow) char[128];
                for (int i = 0; i < NumTotalRegs; i ++){
                    bzero(regValue, 128);
                    
                    for(int curPos = 0; ; curPos++){
                        DEBUG('c', "curPos is %d\n", curPos);
                        execFile -> Read(&regValue[curPos], 1);
                        
                        DEBUG('c', "curPos is %d and val is %c\n", curPos, regValue[curPos]);
                        if (regValue[curPos] == ':'){
                            regValue[curPos] = '\n';
                            break;
                        }
                    }
                    
                    newThread -> userRegisters[i] = (int) strtol(regValue, NULL, 0);
                }
                
                newThread -> userRegisters[2] = 12;

                newThread -> Fork(CheckPointThread, 0);
            }
            
            forkExec -> Release();
            
            KillThread(-12); //Kill self, child will run after finishing exec prep
            
            ASSERT(false); //Should never be reached
            break;
            
        case SC_Dup:
            //Duties entirely delegated to addrspace.cc
            machine -> WriteRegister(2, currentThread -> space -> dupFd(machine -> ReadRegister(4)));

            IncrementPc();
            break;
#endif            
            
        case SC_CheckPoint:
            machine -> WriteRegister(2, 0);
            IncrementPc();
            currentThread -> SaveUserState(); //About 90% on this
            
            arg = new(std::nothrow) char[128];
            checkValue = new(std::nothrow) char[128];
            
            ReadArg(arg, 127, false);
            
            if(!fileSystem -> Create(arg,-1)){
                machine -> WriteRegister(2, -1);
                break;
            }
            
            checkFile = fileSystem -> Open(arg);
            
            //Write cookie
            checkFile -> Write("FUCKNACHOS\n", 11);
            
            //Write num pages
            checkLen = snprintf(checkValue, 128, "%d\n", currentThread -> space -> numPages);
            checkFile -> Write(checkValue, checkLen);
            
            bzero(checkValue, 128);
            
            //Write mem contents
            for (int i = 0; i < currentThread -> space -> numPages; i++){
                DEBUG('c', "Number of pages in checkpointing thread %d\n", currentThread -> space -> numPages);
                vmInfoLock -> Acquire();
                            
                if (currentThread -> space -> pageTable[i].valid){
                    int parentPhysicalPage = currentThread -> space -> pageTable[i].physicalPage;
                   
                    faultInfo[parentPhysicalPage] -> locked = true;
                    
                    for (int j = 0; j < PageSize; j++)
                        checkValue[j] = machine -> mainMemory[parentPhysicalPage * PageSize + j];
                    
                    faultInfo[parentPhysicalPage] -> locked = false;
                    vmInfoLock -> Release();
                }
                
                else{
                    vmInfoLock -> Release();
                    
                    faultLock -> Acquire();
                    megaDisk -> ReadSector(currentThread -> space -> diskSectors[i], checkValue);
                    faultLock -> Release();
                }
                
                checkFile -> Write(checkValue, 128);
                checkFile -> Write(":", 1);
                bzero(checkValue, 128);
            }
            
            checkFile -> Write("\n", 1);
            
            //Write registers
            for(int i = 0; i < NumTotalRegs; i ++){
                checkLen = snprintf(checkValue, 128, "%d:", currentThread -> userRegisters[i]);
                
                checkFile -> Write(checkValue, checkLen);
                bzero(checkValue, 128);
            }
            
            break;

            //Exit and join stuff ignored
            
        default:
            printf("Undefined SYSCALL %d\n", type);
            ASSERT(false);
        }
        
        break;
    

    case PageFaultException:
        DEBUG('v', "PageFaultException\n");
        faultPage(machine -> ReadRegister(BadVAddrReg), false);
        
        break;

    
#ifdef CHANGED
    case NoException: // Everything ok!
        break;
        
    case ReadOnlyException: // Write attmepted to page marked "read-only"
        fprintf(stderr, "ReadOnly\n");
        KillThread(2);
        break;
        
    case BusErrorException: // Translation resulted in an invalid physical address
        fprintf(stderr, "BusError\n");
        KillThread(2);  
        break;
					   
    case AddressErrorException: // Unaligned reference or one that
                                // was eyond the end of the address space
        fprintf(stderr, "AddressError\n");
        KillThread(2);
        break;
					    
    case OverflowException: // Integer overflow in add or sub.
        fprintf(stderr, "overflow\n");
        break;
        
    case IllegalInstrException: // Unimplemented or reserved instr.
        fprintf(stderr, "IllegalInstr\n");
        KillThread(2);
		break;
        
    case NumExceptionTypes:
        fprintf(stderr, "NumExceptionTypes\n");
        break;
    }  
    
    DEBUG('a', "Leaving exception handler\n");
}

void faultPage(int faultingAddr, bool lockBit){
    faultLock -> Acquire();
    
    int faultPage, faultSector, newLocation, victim;
    char* newPage = new char[128];
    FaultData* newData = new FaultData;
    
    stats -> numPageFaults ++;
    
    victim = pageToRemove();
    DEBUG('v', "Victim is %d\n", victim); 
    if (victim == -1){
        bitLock -> Acquire();
        newLocation = memMap -> Find();
        bitLock -> Release();
    }
    
    else{
        
        vmInfoLock -> Acquire();
        Thread* poorThread = faultInfo[victim] -> owner;
        int oldVirtualPage = faultInfo[victim] -> virtualPage;
        int newSector;
        poorThread -> space -> pageTable[oldVirtualPage].valid = false;
        vmInfoLock -> Release();
        
        diskBitLock -> Acquire();
        newSector = diskMap -> Find();
        diskBitLock -> Release();
        
        poorThread -> space -> diskSectors[oldVirtualPage] = newSector;
        
        megaDisk -> WriteSector(newSector, &machine -> mainMemory[victim * PageSize]);
        
        newLocation = victim;
    }
    
        
    faultPage = faultingAddr / PageSize;
    faultSector = currentThread -> space -> diskSectors[faultPage];
    DEBUG('v', "Fault addr: %d, fault page: %d, going to: %d\n", faultingAddr, faultPage, newLocation); 
    
    newData -> owner = currentThread;
    newData -> virtualPage = faultPage;
    newData -> locked = lockBit;
    
    vmInfoLock -> Acquire();
    faultInfo[newLocation] = newData;
    vmInfoLock -> Release();
    
    megaDisk -> ReadSector(faultSector, newPage);
    
    DEBUG('v', "Clearing %d in diskMap\n", faultSector);
    diskBitLock -> Acquire();
    diskMap -> Clear(faultSector);
    diskBitLock -> Release();
    
    for (int i = 0; i < PageSize; i++){
        machine -> mainMemory[newLocation * PageSize +  i] = newPage[i];
    }
    
    vmInfoLock -> Acquire(); 
    currentThread -> space -> pageTable[faultPage].valid = true;  
    currentThread -> space -> pageTable[faultPage].use = true;
    currentThread -> space -> pageTable[faultPage].physicalPage = newLocation;
    vmInfoLock -> Release();
    
    faultLock -> Release();
}

int pageToRemove(){
    int victim;
    int numNull = 0;
    
    bitLock -> Acquire();
    int numClear  = memMap -> NumClear();
    bitLock -> Release();
    
    //LRU Clock approx
    vmInfoLock -> Acquire();
    FaultData* curData = faultInfo[clockPos];
    victim = -1;
    while (victim == -1 && numNull < NumPhysPages && numClear == 0){
        if (curData != NULL){
            int curVirtPage = curData -> virtualPage;
            
            if (curData -> owner -> space -> pageTable[curVirtPage].use)
                curData -> owner -> space -> pageTable[curVirtPage].use = 0;
            
            else{
                if (!curData -> locked)
                    victim = clockPos;
            }
        }
        else
            numNull++;
        
        clockPos = (clockPos + 1) % NumPhysPages;
        curData = faultInfo[clockPos];
       
    }
    vmInfoLock -> Release();

    DEBUG('v', "Num clear is: %d\n", memMap -> NumClear());
    if (numClear != 0){
       
        return -1;
    }
    
    return victim;
}

void bringIntoMemory(int virtualAddr){
    vmInfoLock -> Acquire();
    int virtualPage = virtualAddr / PageSize;
    DEBUG('v', "Virtual addr is %d\n", virtualAddr);
   
    if (currentThread -> space -> pageTable[virtualPage].valid){
        faultInfo[currentThread -> space -> pageTable[virtualPage].physicalPage] -> locked = true;
        vmInfoLock -> Release();
    }
    
    else{
        DEBUG('v', "Fault from bring into memory\n");
        vmInfoLock -> Release();
        faultPage(virtualAddr, true);
    }
}

/*
* Pulls "size" number of characters from register 4, and puts them into
* the local buffer "result".
*/
void ReadArg(char* result, int size, bool write){ //Size refers to last index of array
    int location, physAddr;
    location = machine->ReadRegister(4);

    int currentPage = location / PageSize;
    DEBUG('v', "Bring into memory from readarg\n");
    bringIntoMemory(location); 
    
    for (int i = 0; i < size; i++){
        if (location / PageSize != currentPage){
            vmInfoLock -> Acquire();
            faultInfo[currentThread -> space -> pageTable[(location - 1) / PageSize].physicalPage] -> locked = false;
            vmInfoLock -> Release();
            
            currentPage = location / PageSize;
            DEBUG('v', "Bring into memory from readarg in for loop\n");
            bringIntoMemory(location);
        }
        
        physAddr = ConvertAddr(location);
       
        if ((result[i] = machine->mainMemory[physAddr]) == '\0')
            break;
        location++;
    }
    
    if (!write)
       result[size] = '\0';
    
    vmInfoLock -> Acquire();
    faultInfo[currentThread -> space -> pageTable[location / PageSize].physicalPage] -> locked = false;
    vmInfoLock -> Release();         
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

/*
 * Converts a VA to a PA using the currentThread's pageTable
 */
int ConvertAddr (int virtualAddress){
    int virtPage = virtualAddress / PageSize;
    int offset = virtualAddress % PageSize;
    
    DEBUG('v', "Current thread's numpages is %d, virtualAddress is %d and virtPage is %d\n", currentThread -> space -> numPages, virtualAddress, virtPage); 
    vmInfoLock -> Acquire();
    int retVal =  ((currentThread -> space -> pageTable[virtPage].physicalPage) * PageSize) + offset;
    vmInfoLock -> Release();
    
    return retVal;
}

/*
 * Finishes prepping newly forked child, and puts it in execution.
 */
void CopyThread(int garbage){

    forkSem -> V(); //Wake up parent
     
    currentThread -> space -> RestoreState(); //Puts page table into machine
    
    currentThread -> RestoreUserState(); //Put thread registers into the machine

    machine -> Run();
}

/*
 * Finishes prepping newly execed process and puts it in execution.
 */
void ExecThread(int argsInt){

    currentThread -> space -> RestoreState(); //Put pageTable in machine
    currentThread -> space -> InitRegisters(); //Initialize registers as if it were a new process
    
    /*
     * "argsInt" is a pointer to the exec args we pulled into kernel memory in exec. They will now be put 
     * back into memory for the execed process as arguments to main.
     */
    if (argsInt != 0){ 
        char** args = (char**) argsInt;
        int argc = 0;
        
        while (args[argc] != NULL)
            argc ++;
        
        int* argAddrs = new(std::nothrow) int[argc]; //Will contain VAs of each argument 
        int sp = machine -> ReadRegister(StackReg); 
        int len;  
        
        for (int i = 0; i < argc; i ++){ //For each argument, put it into memory byte by byte
            len = strlen(args[i]) + 1; //Including null byte
            sp -= len;
            
            int currentPage = sp / PageSize;
            bringIntoMemory(sp);
            
            int location = sp;
            for (int j = 0; j < len; j++){
                location = sp + j;
                
                if (location / PageSize != currentPage){
                    vmInfoLock -> Acquire();
                    faultInfo[currentThread -> space -> pageTable[(location - 1) / PageSize].physicalPage] -> locked = false;
                    vmInfoLock -> Release();
                    
                    currentPage = location / PageSize;
                    bringIntoMemory(location);
                }
                
                machine -> mainMemory[ConvertAddr(sp + j)] = args[i][j];   
            }
            
            argAddrs[i] = sp; //VA of arg str
            
            vmInfoLock -> Acquire();
            faultInfo[currentThread -> space -> pageTable[location / PageSize].physicalPage] -> locked = false;
            vmInfoLock -> Release();   
        }
        
        sp = sp & ~3; //Align for ints
        
        sp -= sizeof(int) * (argc); //Move sp for each pointer to each arg str
        
        int currentPage = sp / PageSize;
        bringIntoMemory(sp);
        int location = sp;
        
        for (int i = 0; i < argc; i ++){
            location = sp + i*4;
        
            if (location / PageSize != currentPage){
                vmInfoLock -> Acquire();
                faultInfo[currentThread -> space -> pageTable[(location - 4) / PageSize].physicalPage] -> locked = false;
                vmInfoLock -> Release();
                
                currentPage = location / PageSize;
                DEBUG('v', "Bring into mem in second outer for loop in exec thread\n");
                bringIntoMemory(location);
            }
            
            *(unsigned int *) &machine -> mainMemory[ConvertAddr(sp + i*4)] = WordToMachine((unsigned int) argAddrs[i]); //Put pointer in memory
        }
        
        DEBUG('v', "Location after for: %d, physPage is %d\n", location, currentThread -> space -> pageTable[location / PageSize].physicalPage);
        
        vmInfoLock -> Acquire();
        faultInfo[currentThread -> space -> pageTable[location / PageSize].physicalPage] -> locked = false;
        vmInfoLock -> Release();   
        
        machine -> WriteRegister(4, argc); 
        machine -> WriteRegister(5, sp); //Pointer to pointers to exec args
        machine -> WriteRegister(StackReg,sp - 8);
    }
    
    currentThread -> SaveUserState(); //Ensure that registers are saved in case of context switch

    machine -> Run();  
}

/*
 * Called when a thread exits or has execed a new process. Returns memory to the bitmap, updates joinlist and kills thread.
 */
void KillThread(int exitVal){
    
    DEBUG('v', "In kill thread\n");
    
    AddrSpace* space = currentThread -> space;

    for (int i = 0; i < space -> numPages; i ++){
        bitLock -> Acquire(); 
        vmInfoLock -> Acquire();
        
        if (space -> pageTable[i].valid){
            memMap -> Clear(space -> pageTable[i].physicalPage); //Release each page back to the bitmap
            faultInfo[space -> pageTable[i].physicalPage] = NULL;
        }
        else{
            diskBitLock -> Acquire();
            diskMap -> Clear(space -> diskSectors[i]);
            diskBitLock -> Release();
        }
        
        vmInfoLock -> Release();
        bitLock -> Release();
    }

    if (exitVal != -12){ //Arbitrary value used to determine if function is called by exec. Should not do join stuff in that case.
        joinSem -> P();
        JoinNode* joinNode = joinList -> getNode((Thread*) space -> parentThreadPtr, space -> mySpaceId); //Gets node for global join list. Uses values stored in addrspace during fork.
        joinSem -> V();
        
        if (joinNode != NULL){
            joinNode -> exitVal = exitVal;
            joinNode -> permission -> V();
        }
    }
  
    currentThread -> Finish(); //Kill thread
}

/*
 * Copies exec arguments from user memory into kernel buffer execArgs from VA argAddr.
 */
void CopyExecArgs(char** execArgs, int argAddr){
    int str, argc, physAddr;
    
    if (argAddr != 0){
        argc = 0;
        
        int currentPage = argAddr / PageSize;
        bringIntoMemory(argAddr);
        int location;
        
        physAddr = ConvertAddr(argAddr);
        str = *(unsigned int *) &machine -> mainMemory[physAddr];
        
        while (str != 0){ //str is a VA (pointer) where the exec arg strings reside
            int count = 0;
            
            int currentStrPage = str / PageSize;
            int pagesBroughtIn[128] = {-1};
            int curPageIn = 1;
            
            bringIntoMemory(str);
            
            int curChar = ConvertAddr((int)str); //Physical address of char pointed to by str
            pagesBroughtIn[0] = currentStrPage;
            
            while (machine -> mainMemory[curChar] != '\0'){ //Count how big the string is
                str ++;
                
                if (str / PageSize != currentStrPage){
                    bringIntoMemory(str);
                    currentStrPage = str / PageSize;
                    pagesBroughtIn[curPageIn] = currentStrPage;
                    
                    curPageIn++;
                }
                
                curChar = ConvertAddr((int) str);
                count ++;
            }
            
            if (argc >= 128)//Args over 128 get truncated
                break;
               
            execArgs[argc] = new(std::nothrow) char[count + 1]; //Allocate space for argument
            
            
            str = str - count; //First char 
            curChar = ConvertAddr((int)str);
            count = 0;
            
            while (machine -> mainMemory[curChar] != '\0'){ //Actually copy the str into the execArgs buffer
                execArgs[argc][count] = machine -> mainMemory[curChar];
                str ++;
                curChar = ConvertAddr((int) str);
                count ++;
            }
            
            vmInfoLock -> Acquire(); 
            for (int i = 0; i < 128 && pagesBroughtIn[i] != -1; i ++)
                faultInfo[currentThread -> space -> pageTable[pagesBroughtIn[i]].physicalPage] -> locked = false;
            vmInfoLock -> Release();

            execArgs[argc][count] = '\0';
            DEBUG('v', "Argument %d is %s\n", argc, execArgs[argc]);
             
            argc ++;
            
            argAddr += 4; //Go to next pointer
            location = argAddr;
            
            if (location / PageSize != currentPage){
                vmInfoLock -> Acquire();
                faultInfo[currentThread -> space -> pageTable[currentPage].physicalPage] -> locked = false;
                vmInfoLock -> Release();

                currentPage = location / PageSize;
                bringIntoMemory(location);
            }
            
            physAddr = ConvertAddr(argAddr);
            str = *(unsigned int *) &machine -> mainMemory[physAddr];
        }

        vmInfoLock -> Acquire();
        faultInfo[currentThread -> space -> pageTable[location / PageSize].physicalPage] -> locked = false;
        vmInfoLock -> Release();   
        
        execArgs[argc] = NULL; //Last arg is NULL ptr
    }
    
    else
        execArgs[0] = NULL; //No arguments  
        
}

void CheckPointThread(int garbage){
    DEBUG('c', "IN CHECKPOINT THREAD\n");
    currentThread -> space -> RestoreState();
    currentThread -> RestoreUserState();
    
    machine -> Run();
    
}

#endif
