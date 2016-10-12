#include "synchconsole.h"


SynchConsole(char* fileReader, char* fileWriter)
{
    console = new Console(fileReader, fileWriter, readSeek, writeSeek, this);
    readLock = new Lock("Reader lock for Console Synch");
    writeLock = new Lock"Writer lock for Console Synch");
    readSem = new Semaphore("Reader semaphore for Console Synch);
    writeSem = new Semaphore("Writer semaphore for Console Synch);
    



SynchConsole::~SynchConsole() //Purge SynchConsole
{
    delete console;
    delete readSem;
    delete writeSem;
    delete readLock;
    delete writeLock;
}
