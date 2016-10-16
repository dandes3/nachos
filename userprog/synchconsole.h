#ifndef SYNCHCONSOLE_H
#define SYNCHCONSOLE_H

#include "console.h"
#include "synch.h"


class SynchConsole
{
    public:
        SynchConsole(char* fileReader, char* fileWriter);
        ~SynchConsole();
        
        void GetChar(char* cp);
        void PutChar(char c);
        void ReadAvail();
        void WriteDone();
        
    private:
        Console* console;
        Semaphore *readSem, *writeSem;
        Lock *readLock, *writeLock;

};

#endif
