// threadtest.cc 
//	Simple test case for the threads assignment.
//
//	Create two threads, and have them context switch
//	back and forth between themselves by calling Thread::Yield, 
//	to illustratethe inner workings of the thread system.
//
// Copyright (c) 1992-1993 The Regents of the University of California.
// All rights reserved.  See copyright.h for copyright notice and limitation 
// of liability and disclaimer of warranty provisions.

#include "copyright.h"
#include "system.h"
#include "elevatormanager.h"

int elevatorPos = 0;
int usersDelivered = 0;

Semaphore *countMutex = new(std::nothrow) Semaphore("countMutex", 1);
Semaphore *elevatorTravel = new(std::nothrow) Semaphore("elevatorTravel", 0);

Thread *threads[10];
char tNames[10][5];

Thread *elevatorThread;
Semaphore *elevatorSem;

ElevatorManager *manager;
//----------------------------------------------------------------------
// SimpleThread
// 	Loop 5 times, yielding the CPU to another ready thread 
//	each iteration.
//
//	"which" is simply a number identifying the thread, for debugging
//	purposes.
//----------------------------------------------------------------------

void
SimpleThread(int which)
{
    int num;
    
    for (num = 0; num < 5; num++) {
	   printf("*** thread %d looped %d times\n", which, num);
      currentThread->Yield();
    }
}

//----------------------------------------------------------------------
// ThreadTest
// 	Set up a ping-pong between two threads, by forking a thread 
//	to call SimpleThread, and then calling SimpleThread ourselves.
//----------------------------------------------------------------------

void
ThreadTest()
{
    DEBUG('t', "Entering SimpleTest");

    Thread *t = new(std::nothrow) Thread("forked thread");

    t->Fork(SimpleThread, 1);
    SimpleThread(0);
}

void Elevator(int arg){

   while (usersDelivered < 20){ //While a thread is still outstanding to be delivered
      elevatorSem -> P();//Wait for a thread to execute a request

      for (int i = 0; i < 100; i++){ //Elevator delay
         interrupt -> SetLevel(IntOff);
         interrupt -> SetLevel(IntOn);
      }
      
     
     manager -> Release(); //Signals a thread to wake from its wait in Request. Fix for mesa semantics can be found in elevatormanager.cc
     elevatorTravel -> V();
    }
}

void ArrivingGoingFromTo(int atFloor, int toFloor, int num){
   
   printf("p%d is requesting to be pickedup at floor %d\n", num, atFloor);
  
   manager -> Request(false, num, atFloor); //Request pickup. False value to indicate pickup rather than dropoff

   printf("p%d picked up at floor %d------------------------------------%d\n", num, atFloor, atFloor);

   elevatorSem -> V(); //Signal elevator to "move" and call Release
   
   elevatorTravel -> P(); //Wait for elevator delay.

   printf("p%d requesting to be dropped off at floor %d\n", num, toFloor);

   manager -> Request(true, num, toFloor); //Request floor for dropoff after elevator arrives. On return, thread has been delivered at desired floor
   
   printf("p%d dropped off at floor %d----------------------------------%d\n", num, toFloor, toFloor);

   countMutex -> P(); //Mutual exclusion for usersDelivered variable
   usersDelivered ++; //Increment number of threads delivered
   countMutex -> V();
   
   manager -> Release();
}

void ElevatorUser(int num){

   int delay = 1+(int) (600.0*rand()/(RAND_MAX+1.0)); //Random delay, set within 600 to better insure overlap of requests
   int fromFloor = rand() % 4; //Random floor to request a pickup
   int toFloor;

   while ((toFloor = rand() % 4) == fromFloor); //Random floor to request dropoff, cannot be the same as from floor

   for (int i=0; i<delay; i++){ //Execute delay
     interrupt->SetLevel(IntOff);
     interrupt->SetLevel(IntOn);
   }
   
   ArrivingGoingFromTo(fromFloor, toFloor, num);

}

void ElevatorTest(){

   int i;

   manager = new(std::nothrow) ElevatorManager(); //Monitor

   elevatorThread = new(std::nothrow) Thread("elevator"); //Thread to simulate the elevator
   elevatorSem = new(std::nothrow) Semaphore("elevator", 0); //Semaphore to tell elevator to release a thread

   elevatorThread -> Fork(Elevator,0);

   for (i = 0; i < 10; i++){ //Spawn ten threads to use the elevator
      sprintf(tNames[i], "p%d", i);
      threads[i] = new(std::nothrow) Thread(tNames[i]);
      threads[i] -> Fork(ElevatorUser, i);
   }
}
   
