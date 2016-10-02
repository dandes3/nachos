// synch.cc 
//    Routines for synchronizing threads.  Three kinds of
//    synchronization routines are defined here: semaphores, locks 
//       and condition variables (the implementation of the last two
//    are left to the reader).
//
// Any implementation of a synchronization routine needs some
// primitive atomic operation.  We assume Nachos is running on
// a uniprocessor, and thus atomicity can be provided by
// turning off interrupts.  While interrupts are disabled, no
// context switch can occur, and thus the current thread is guaranteed
// to hold the CPU throughout, until interrupts are reenabled.
//
// Because some of these routines might be called with interrupts
// already disabled (Semaphore::V for one), instead of turning
// on interrupts at the end of the atomic operation, we always simply
// re-set the interrupt state back to its original value (whether
// that be disabled or enabled).
//
// Copyright (c) 1992-1993 The Regents of the University of California.
// All rights reserved.  See copyright.h for copyright notice and limitation 
// of liability and disclaimer of warranty provisions.

#include "copyright.h"
#include "synch.h"
#include "system.h"

//----------------------------------------------------------------------
// Semaphore::Semaphore
//     Initialize a semaphore, so that it can be used for synchronization.
//
//    "debugName" is an arbitrary name, useful for debugging.
//    "initialValue" is the initial value of the semaphore.
//----------------------------------------------------------------------

Semaphore::Semaphore(const char* debugName, int initialValue)
{
    name = debugName;
    value = initialValue;
    queue = new(std::nothrow) List;
}

//----------------------------------------------------------------------
// Semaphore::Semaphore
//     De-allocate semaphore, when no longer needed.  Assume no one
//    is still waiting on the semaphore!
//----------------------------------------------------------------------

Semaphore::~Semaphore()
{
    delete queue;
}

//----------------------------------------------------------------------
// Semaphore::P
//     Wait until semaphore value > 0, then decrement.  Checking the
//    value and decrementing must be done atomically, so we
//    need to disable interrupts before checking the value.
//
//    Note that Thread::Sleep assumes that interrupts are disabled
//    when it is called.
//----------------------------------------------------------------------

void
Semaphore::P()
{
    IntStatus oldLevel = interrupt->SetLevel(IntOff);    // disable interrupts
    
    while (value == 0) {     		// semaphore not available
    queue->Append((void *)currentThread);	// so go to sleep
    currentThread->Sleep();
    } 
    value--;     				// semaphore available, 
    					// consume its value
    
    (void) interrupt->SetLevel(oldLevel);    // re-enable interrupts
}

//----------------------------------------------------------------------
// Semaphore::V
//     Increment semaphore value, waking up a waiter if necessary.
//    As with P(), this operation must be atomic, so we need to disable
//    interrupts.  Scheduler::ReadyToRun() assumes that threads
//    are disabled when it is called.
//----------------------------------------------------------------------

void
Semaphore::V()
{
    Thread *thread;
    IntStatus oldLevel = interrupt->SetLevel(IntOff);

    thread = (Thread *)queue->Remove();
    if (thread != NULL)       // make thread ready, consuming the V immediately
    scheduler->ReadyToRun(thread);
    value++;
    (void) interrupt->SetLevel(oldLevel);
}

/*
*   Lock implementation without the use of semaphores or busy waiting. Derived
*   strongly from above semaphore implementation. Implemented essentially as a 
*   binary semaphore.
*
*   By: Philip Favaloro
*/

Lock::Lock(const char* debugName) {
    name = debugName; 
    status = true; //Locked or unlocked
    queue = new(std::nothrow) List; //Sleepers
    lockHolder = NULL; //For checking if currentThread holds the lock
}

Lock::~Lock() {
    delete queue;
}

void Lock::Acquire() {
    IntStatus oldLevel = interrupt->SetLevel(IntOff); //Atomic actions

    while (!status) { //Mesa semantics
    	queue->Append((void*)currentThread);
    	currentThread->Sleep();
    }

    status = false; //Lock is held

    lockHolder = currentThread;

    (void)interrupt->SetLevel(oldLevel);
}

void Lock::Release() {
    IntStatus oldLevel = interrupt->SetLevel(IntOff);

    ASSERT(isHeldByCurrentThread()); //Cannot release a lock if not currently held

    Thread *thread;

    thread = (Thread *)queue->Remove();
    if (thread != NULL)
    	scheduler->ReadyToRun(thread); //Put on ready list, Mesa semantics

    status = true; //Lock free

    (void)interrupt->SetLevel(oldLevel);
}

bool Lock::isHeldByCurrentThread() {
    return lockHolder == currentThread;
}

/*
*  Condition variables built using locks and interrupt disabling. Implements
*  a priority wait and queue-is-empty function in addition to the required 
*  functionality.
*
*  By Philip Favaloro
*/

Condition::Condition(const char* debugName) {
    name = debugName;
    queue = new(std::nothrow) List;
}

Condition::~Condition() {
    delete queue;
}

void Condition::Wait(Lock* conditionLock) {
    IntStatus oldLevel = interrupt->SetLevel(IntOff); //Atomic actions

    ASSERT(conditionLock->isHeldByCurrentThread()); //Cannot wait on a lock not currently held

    //Release lock and get put on sleepers list
    conditionLock->Release(); 
    queue->SortedInsert((void*)currentThread, std::numeric_limits<long long>::max()); // Uses max long long as priority to ensure all
                                                                                      // threads waiting without a priority are woken after 
    currentThread->Sleep(); // Relinquish cpu                                         // threads with a priority
    conditionLock->Acquire(); //Will acquire the lock when awoken by signal

    (void)interrupt->SetLevel(oldLevel);
}

void Condition::Wait(Lock* conditionLock, long long priority){
	//Exactly the same as above wait, but prioirty is used for sorted insert

        IntStatus oldLevel = interrupt->SetLevel(IntOff);

        ASSERT(conditionLock->isHeldByCurrentThread());

        conditionLock->Release();
        queue->SortedInsert((void*)currentThread, priority);
    
        currentThread->Sleep();
        conditionLock->Acquire();

        (void)interrupt->SetLevel(oldLevel);
}

void Condition::Signal(Lock* conditionLock) {
    IntStatus oldLevel = interrupt->SetLevel(IntOff);

    ASSERT(conditionLock->isHeldByCurrentThread());

    Thread *thread;

    thread = (Thread *)queue->SortedRemove(NULL); //Remove item from list with smallest priority
    if (thread != NULL)
    	scheduler->ReadyToRun(thread); //Place thread on ready list

    (void)interrupt->SetLevel(oldLevel);
}

void Condition::ThreadWaiting(bool* ret){
    IntStatus oldLevel = interrupt->SetLevel(IntOff);
  
    *ret = !queue->IsEmpty(); //Puts result in variable rather than return value to preserve synchronization

   (void)interrupt->SetLevel(oldLevel);
}

void Condition::Broadcast(Lock* conditionLock) {
    IntStatus oldLevel = interrupt->SetLevel(IntOff);

    ASSERT(conditionLock->isHeldByCurrentThread());

    Thread *thread;

    thread = (Thread *)queue->SortedRemove(NULL);

    while (thread != NULL){//Iterates through each thread in the queue
    	scheduler->ReadyToRun(thread); //Puts each thread on ready list
    	thread = (Thread*) queue->Remove();
    }

    (void)interrupt->SetLevel(oldLevel);
}
