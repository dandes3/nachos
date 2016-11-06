#ifndef SYNCHCONSOLE_H
#define SYNCHCONSOLE_H

#ifdef CHANGED
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
        void setConsole(char *fileIn, char *fileOut);
    private:
        Console* console;
        Semaphore *readSem, *writeSem;
        Lock *readLock, *writeLock ;

};

#endif
#endif
