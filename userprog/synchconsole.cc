#include "synchconsole.h"


void SynchConsole::ReadAvail() //Release the Read Semaphore
{
    readSem->V();
}

void SynchConsole::WriteDone() //Release the Write Semaphore 
{
    writeSem->V();
}

static void readDummy(void* pass) //Dummy read pointer
{
    SynchConsole* synchConsole = (SynchConsole*) pass;
    synchConsole->ReadAvail();
}

static void writeDummy(void* pass) //Dummy write pointer
{
    SynchConsole* synchConsole = (SynchConsole*) pass;
    synchConsole->WriteDone();
}


SynchConsole(char* fileReader, char* fileWriter) //Initializes usage locks and semaphores
{
    console = new Console(fileReader, fileWriter, readDummy, writeDummy, this); //Build new console, "this" is a passed location identifier
                                                                              // readDummy and writeDummy passed as void pointers passed as
                                                                              // requirements for console creation 
    readLock = new Lock("Reader lock for Console Synch");
    writeLock = new Lock"Writer lock for Console Synch");
    readSem = new Semaphore("Reader semaphore for Console Synch");
    writeSem = new Semaphore("Writer semaphore for Console Synch");
}

SynchConsole::~SynchConsole() //Purge SynchConsole
{
    delete console;
    delete readSem;
    delete writeSem;
    delete readLock;
    delete writeLock;
}

char SynchConsole::GetChar() //Lock the readlock and hang Read Semaphore, pull from console, return character
{
    readLock->Acquire();
    readSem->P();
    char c = console->GetChar();
    readLock->Release();
    return c;
}

void SynchConsole::PutChar(char c) //Lock the writelock and push character, hang Write Semaphore and release lock
{
    writeLock->Acquire();
    console->PutChar(c);
    writeSem->P();
    writeLock->Release();
}

