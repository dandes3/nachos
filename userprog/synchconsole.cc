#include "synchconsole.h"

#ifdef CHANGED
void SynchConsole::ReadAvail() //Release the Read Semaphore
{
    readSem->V();
}

void SynchConsole::WriteDone() //Release the Write Semaphore 
{
    writeSem->V();
}

static void SConsoleReadAvail(int s) 
{ SynchConsole *sConsole = (SynchConsole *)s; sConsole->ReadAvail(); }
static void SConsoleWriteAvail(int s) 
{ SynchConsole *sConsole = (SynchConsole *)s; sConsole->WriteDone(); }

SynchConsole::SynchConsole(char* fileReader, char* fileWriter) //Initializes usage locks and semaphores
{
    console = new Console(fileReader, fileWriter, (VoidFunctionPtr) SConsoleReadAvail, (VoidFunctionPtr) SConsoleWriteAvail, (int)this); //Build new console, "this" is a passed location identifier
                                                                              // readDummy and writeDummy passed as void pointers passed as
                                                                              // requirements for console creation 
    readLock = new Lock("Reader lock for Console Synch"); //Likely overkill, ensure atomicity of writes and reads
    writeLock = new Lock("Writer lock for Console Synch");
    readSem = new Semaphore("Reader semaphore for Console Synch", 0);
    writeSem = new Semaphore("Writer semaphore for Console Synch", 0);
}

SynchConsole::~SynchConsole() //Purge SynchConsole
{
    delete console;
    delete readSem;
    delete writeSem;
    delete readLock;
    delete writeLock;
}

void SynchConsole::GetChar(char* cp) //Lock the readlock and hang Read Semaphore, pull from console, return character
{
    readLock->Acquire();
    readSem -> P();
    
    char c = console->GetChar();
    console -> CheckCharAvail();
    
    *cp = c;
    readLock->Release();
}

void SynchConsole::PutChar(char c) //Lock the writelock and push character, hang Write Semaphore and release lock
{
    writeLock->Acquire();
    console->PutChar(c);
    writeSem -> P();
    writeLock->Release();
}

void SynchConsole::setConsole(char *fileIn, char *fileOut){
    console = new Console(fileIn, fileOut, (VoidFunctionPtr) SConsoleReadAvail, (VoidFunctionPtr) SConsoleWriteAvail, (int)this);
}

#endif
