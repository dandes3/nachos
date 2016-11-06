
// addrspace.cc 
//	Routines to manage address spaces (executing user programs).
//
//	In order to run a user program, you must:
//
//	1. link with the -N -T 0 option 
//	2. run coff2noff to convert the object file to Nachos format
//		(Nachos object code format is essentially just a simpler
//		version of the UNIX executable object code format)
//	3. load the NOFF file into the Nachos file system
//		(if you haven't implemented the file system yet, you
//		don't need to do this last step)
//
// Copyright (c) 1992-1993 The Regents of the University of California.
// All rights reserved.  See copyright.h for copyright notice and limitation 
// of liability and disclaimer of warranty provisions.

#include "copyright.h"
#include "system.h"
#include "addrspace.h"
#include "noff.h"
#include <new>

#ifdef CHANGED

#define SCRIPT 0x52435323 //Used to identify a shell script being brought into execution
extern void StartProcess(char *filename, char *inputName);
//----------------------------------------------------------------------
// SwapHeader
// 	Do little endian to big endian conversion on the bytes in the 
//	object file header, in case the file was generated on a little
//	endian machine, and we're now running on a big endian machine.
//----------------------------------------------------------------------

static void 
SwapHeader (NoffHeader *noffH)
{
	noffH->noffMagic = WordToHost(noffH->noffMagic);
	noffH->code.size = WordToHost(noffH->code.size);
	noffH->code.virtualAddr = WordToHost(noffH->code.virtualAddr);
	noffH->code.inFileAddr = WordToHost(noffH->code.inFileAddr);
	noffH->initData.size = WordToHost(noffH->initData.size);
	noffH->initData.virtualAddr = WordToHost(noffH->initData.virtualAddr);
	noffH->initData.inFileAddr = WordToHost(noffH->initData.inFileAddr);
	noffH->uninitData.size = WordToHost(noffH->uninitData.size);
	noffH->uninitData.virtualAddr = WordToHost(noffH->uninitData.virtualAddr);
	noffH->uninitData.inFileAddr = WordToHost(noffH->uninitData.inFileAddr);
}

//----------------------------------------------------------------------
// AddrSpace::AddrSpace
// 	Create an address space to run a user program.
//	Load the program from a file "executable", and set everything
//	up so that we can start executing user instructions.
//
//	Assumes that the object code file is in NOFF format.
//
//	First, set up the translation from program memory to physical 
//	memory.  For now, this is really simple (1:1), since we are
//	only uniprogramming, and we have a single unsegmented page table
//
//	"executable" is the file containing the object code to load into memory
//----------------------------------------------------------------------

AddrSpace::AddrSpace(OpenFile *executable)
{
    failed = false; //Flag used to tell exception.cc if an addrspace creation failed
    
    stdOut = new(std::nothrow) OpenFile(1); //Cookies for ConsoleOutput and ConsoleInput
    stdIn = new(std::nothrow) OpenFile(0);

    NoffHeader noffH;
    unsigned int size;
#ifndef USE_TLB
    unsigned int i;
#endif

    if (executable == NULL){
        failed = true;
        return;
    }
    
    executable->ReadAt((char *)&noffH, sizeof(noffH), 0);
    if (noffH.noffMagic == SCRIPT){ 
        StartProcess("test/shell", executable->fileName);
        failed = true; //Shouldn't be reached unless something bad happens
        return;
    }
  
    if ((noffH.noffMagic != NOFFMAGIC) && 
		(WordToHost(noffH.noffMagic) == NOFFMAGIC))
    	SwapHeader(&noffH);
    
    if (noffH.noffMagic != NOFFMAGIC){
        failed = true;
        return;
    }

    size = noffH.code.size + noffH.initData.size + noffH.uninitData.size 
			+ UserStackSize;	// we need to increase the size
						// to leave room for the stack
    numPages = divRoundUp(size, PageSize);
    size = numPages * PageSize;

    if (numPages > NumPhysPages){
       failed = true; 
       return;      // check we're not trying
    }				// to run anything too big --
					// at least until we have
					// virtual memory

    DEBUG('a', "Initializing address space, num pages %d, size %d\n", 
					numPages, size);
#ifndef USE_TLB
// first, set up the translation 
    pageTable = new(std::nothrow) TranslationEntry[numPages];
    
    for (i = 0; i < numPages; i++) {
	   pageTable[i].virtualPage = i;
	   
	   bitLock -> Acquire();
       
	   if ((pageTable[i].physicalPage = memMap -> Find()) == -1){ //Grab physical page using bitmap
           failed = true; //If no pages are left, cannot create addrspace
           return;
       }
       bzero(machine -> mainMemory + pageTable[i].physicalPage * PageSize, PageSize); //Zero out memory for safety
       
       bitLock -> Release();
    
	   pageTable[i].valid = true;
	   pageTable[i].use = false;
	   pageTable[i].dirty = false;
	   pageTable[i].readOnly = false;  // if the code segment was entirely on 
					// a separate page, we could set its 
					// pages to be read-only
    }
#endif    

// then, copy in the code and data segments into memory byte by byte, converting to physical memory for each byte
      if (noffH.code.size > 0) {

        DEBUG('a', "Initializing code segment, at 0x%x, size %d\n", noffH.code.virtualAddr, noffH.code.size);
        
        for (int j = 0; j < noffH.code.size; j ++) 
            executable->ReadAt(&(machine->mainMemory[convertVirtualtoPhysical(noffH.code.virtualAddr + j)]), 1, noffH.code.inFileAddr + j); 
    }
       
    if (noffH.initData.size > 0) {
        
        DEBUG('a', "Initializing data segment, at 0x%x, size %d\n", noffH.initData.virtualAddr, noffH.initData.size);
        
        for (int j = 0; j < noffH.initData.size; j ++)
            executable->ReadAt(&(machine->mainMemory[convertVirtualtoPhysical(noffH.initData.virtualAddr + j)]), 1, noffH.initData.inFileAddr + j);
        
    }
    
    for (int j = 0; j < 20; j++) //Init fileVector to all NULL
        fileVector[j] = NULL;

    fileVector[0] = stdIn; //Start with ConsoleInput at 0 and ConsoleOutput at 1
    fileVector[1] = stdOut;
}


//----------------------------------------------------------------------
// AddrSpace::AddrSpace
//   Alternate constructor used in the Fork syscall. Copies memory and 
//   the fileVector from the copySpace.
//
//   "copySpace" is the parent's AddrSpace of the newly forked child
//----------------------------------------------------------------------
AddrSpace::AddrSpace (AddrSpace* copySpace){
    failed = false;
    
    stdOut = copySpace -> stdOut; //Cookie must be the same, its identity is checked by its mem address
    stdIn = copySpace -> stdIn;
    unsigned int i;
    
    //Copy OpenFile objects and update the number of links to the OpenFile instance
    for (i = 0; i < 20; i++){
        if ((fileVector[i] = copySpace -> fileVector[i]) != NULL){
            fileVector[i] -> linkLock -> Acquire();
            fileVector[i] -> links ++;
            fileVector[i] -> linkLock -> Release();
        }
    }
    
    numPages = copySpace -> numPages;
    
    pageTable = new(std::nothrow) TranslationEntry[numPages];
    
    for (i = 0; i < numPages; i++) {
	   pageTable[i].virtualPage = i;
       
	   bitLock -> Acquire();
       
	   if ((pageTable[i].physicalPage = memMap -> Find()) < 0){ //Same as in normal AddrSpace constructor
               failed = true;
                return;
           }
       bzero(machine -> mainMemory + (pageTable[i].physicalPage * PageSize), PageSize);
       
       bitLock -> Release();
       
	   pageTable[i].valid = true;
	   pageTable[i].use = false;
	   pageTable[i].dirty = false;
	   pageTable[i].readOnly = false;  // if the code segment was entirely on 
					// a separate page, we could set its 
					// pages to be read-only
    }
 
    //Copy memory from parent's physical memory to child's physical memory
    for (i = 0; i < numPages; i++){
        int curPhysMemAddr = pageTable[i].physicalPage * PageSize;
        int copyPhysMemAddr = copySpace -> pageTable[i].physicalPage * PageSize;
  
        for (int j = 0; j < PageSize; j ++)
            machine -> mainMemory[curPhysMemAddr + j] = machine -> mainMemory[copyPhysMemAddr + j];
        
    }
    
}

//----------------------------------------------------------------------
// AddrSpace::~AddrSpace
// 	Dealloate an address space.  Nothing for now!
// //---------------------------------------------------------------------

AddrSpace::~AddrSpace()
{
#ifndef USE_TLB
   delete pageTable;
#endif
   /*
   for (int i = 0; i < 20; i++)
      delete fileVector[i];
   */
}

//----------------------------------------------------------------------
// AddrSpace::InitRegisters
// 	Set the initial values for the user-level register set.
//
// 	We write these directly into the "machine" registers, so
//	that we can immediately jump to user code.  Note that these
//	will be saved/restored into the currentThread->userRegisters
//	when this thread is context switched out.
//----------------------------------------------------------------------

void
AddrSpace::InitRegisters()
{
    int i;

    for (i = 0; i < NumTotalRegs; i++)
	machine->WriteRegister(i, 0);

    // Initial program counter -- must be location of "Start"
    machine->WriteRegister(PCReg, 0);	

    // Need to also tell MIPS where next instruction is, because
    // of branch delay possibility
    machine->WriteRegister(NextPCReg, 4);

   // Set the stack register to the end of the address space, where we
   // allocated the stack; but subtract off a bit, to make sure we don't
   // accidentally reference off the end!
    machine->WriteRegister(StackReg, numPages * PageSize - 16);
    DEBUG('a', "Initializing stack register to %d\n", numPages * PageSize - 16);
}

//----------------------------------------------------------------------
// AddrSpace::SaveState
// 	On a context switch, save any machine state, specific
//	to this address space, that needs saving.
//
//	For now, nothing!
//----------------------------------------------------------------------

void AddrSpace::SaveState() 
{}

//----------------------------------------------------------------------
// AddrSpace::RestoreState
// 	On a context switch, restore the machine state so that
//	this address space can run.
//
//      For now, tell the machine where to find the page table,
//      IF address translation is done with a page table instead
//      of a hardware TLB.
//----------------------------------------------------------------------

void AddrSpace::RestoreState() 
{
#ifndef USE_TLB
    machine->pageTable = pageTable;
    machine->pageTableSize = numPages;
#endif
}



//----------------------------------------------------------------------
// AddrSpace::fileOpen
//      Opens a file by name given in parameter fileName. Puts resulting
//	OpenFile object in the fileVector at the first open file descriptor.
//
//	Uses the convention that opening the file "/dev/ttyin" opens stdin
//	and opening "/dev/ttyout" opens stdout.
//----------------------------------------------------------------------
OpenFileId AddrSpace::fileOpen(char* fileName){

    OpenFile* newFile;
    
    //Opening connection to console, not actual files
    if (strcmp(fileName, "/dev/ttyin") == 0) //stdin
       newFile = stdIn; 
    
    else if (strcmp(fileName, "/dev/ttyout") == 0) //stdout
       newFile = stdOut;
    
    else //some other file
        newFile = fileSystem -> Open(fileName); 

    if (newFile == NULL)
	   return -1;
    
    newFile->fileName = fileName;
    
    //Put newFile in fileVector at first open spot
    for (int i = 0; i < 20; i++){
        if (fileVector[i] == NULL){
            fileVector[i] = newFile;
            return i; //Index corresponds to file descriptor
        }
    }
       
    return -1;
}

//----------------------------------------------------------------------
// AddrSpace::fileClose
//	Frees OpenFile object corresponding to the given fileId. Also
//	frees slot in file vector for other files.
//----------------------------------------------------------------------
void AddrSpace::fileClose(OpenFileId fileId){
    
    if (fileId > 19 || fileId < 0 || fileVector[fileId] == NULL) //No file to close
        return;
    
    fileVector[fileId] -> linkLock -> Acquire();
    
    fileVector[fileId] -> links --; //A close decrements the number of links to an OpenFile object
    
    if (fileVector[fileId] != stdIn && fileVector[fileId] != stdOut && fileVector[fileId] -> links == 0) //Only delete the OpenFile object if it has 0 links to it.                                                                                                      
        delete fileVector[fileId];                                                                       //Deleting OpenFile object closes file in linux file system.
                                                                                                         //Since the stdIn and stdOut doesn't correspond to actaul files, this would cause errors.
    fileVector[fileId] -> linkLock -> Release();
    
    fileVector[fileId] = NULL; //Free up space in file vector
}

//----------------------------------------------------------------------
// AddrSpace::readWrite
//      Returns the OpenFile object corresponding to the given fileId. Used
//	by the exception handler to read and write from/to files.
//----------------------------------------------------------------------
OpenFile* AddrSpace::readWrite(OpenFileId fileId){
    if  (fileId > 19 || fileId < 0)
        return NULL;
       
    return fileVector[fileId];
}

//----------------------------------------------------------------------
// AddrSpace::isConsoleFile
//      Unelegant solution for allowing the exception handler to recognize
//	if an OpenFile object returned from readWrite is stdIn, stdOut,
//	or some other file.
//----------------------------------------------------------------------
int AddrSpace::isConsoleFile(OpenFile* file){
    
    if (file == stdIn)
       return 0;

    if (file == stdOut)
       return 1;
  
    return -1;
}

//----------------------------------------------------------------------
// AddrSpace::convertVirtualtoPhysical
//      Converts a VA to a PA.
//----------------------------------------------------------------------
int AddrSpace::convertVirtualtoPhysical(int virtualAddr){
    int virtualPage = virtualAddr / PageSize;
    int offset = virtualAddr % PageSize;

    return pageTable[virtualPage].physicalPage * PageSize + offset;    
}

//----------------------------------------------------------------------
// AddrSpace::dupFd
//      Does all of the work in a dup syscall. Returns the newly duped
//   fd. Always goes to the lowest open file descriptor.
//----------------------------------------------------------------------
OpenFileId AddrSpace::dupFd(int fd){
    if (fd < 0 || fd > 19)
        return -1;
    
    if (fileVector[fd] == NULL){ //Can't dup a nonexistent file
        return -1;
    }
    
    //Find earliest open fd, put the given OpenFile object in it, and increment the number of links to that
    //OpenFile.
    for (int i = 0; i< 20; i ++){
        if (fileVector[i] == NULL){
            fileVector[i] = fileVector[fd];
            
            fileVector[i] -> linkLock -> Acquire();
            fileVector[i] -> links ++;
            fileVector[i] -> linkLock -> Release();
            
            return i;
        }
    }
    
    return -1; //No space to dup
}



#endif
