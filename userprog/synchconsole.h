#ifndef SYNCHCONSOLE_H
#define SYNCHCONSOLE_H

#include "console.h"
#include "synch.h"


class SynchConsole
{
    SynchConsole(char* fileReader, char* fileWriter);
    ~SynchConsole();
    
    char GetChar();
    void PutChar(char c);
    void ReadAvail();
    void WriteDone();
    
    Console* console;
    Semaphore *readSem, *writeSem;
    Lock *readLock, *writeLock;

};

#endif