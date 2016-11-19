#ifndef FAULTINFO_H
#define FAULTINFO_H

#ifdef CHANGED
#include "copyright.h"

/* 
 * Struct used in the faultInfo array. Contains info necessary to manage physical memory
 * when moving pages to and from RAM to implement virtual memory.
 */
typedef struct  FaultData{
    Thread* owner;
    int virtualPage;
    bool locked;
} FaultData;

#endif
#endif
