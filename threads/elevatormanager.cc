/*
*  Implements a monitor using Locks and Condtions to synchronize an elevator picking up
*  and dropping off processes. Several adjustments are made to create output to test the 
*  execution. Code based heavily on discussion in class regarding disk reads.
*   
*  By Philip Favaloro
*/

#include "elevatormanager.h"
#include <new>

ElevatorManager::ElevatorManager(){
   lock = new(std::nothrow) Lock("elevatorLock");
   upSweep = new(std::nothrow) Condition("upSweep");
   downSweep = new(std::nothrow) Condition("downSweep");
   
   position = 0;
   current = 0;
   directionUp = true;
   busy = false;
}

ElevatorManager::~ElevatorManager(){

   delete lock;
   delete upSweep;
   delete downSweep;
}

void ElevatorManager::Request(bool dropOff, int pid, int floor){

   lock -> Acquire(); //Monitor mutual exclusion

   ASSERT(floor < 4 && floor >= 0); //Floor not out of bounds
   
   current ++;               //Safe guard for issues caused by Mesa Semantics. If a thread is waiting on the lock at the top of Request when another
                             //thread has been signaled, and that thread aquires the lock, it will "skip the line" and ruin elevator order. Current tracks the
  if (!busy and current > 1) //number of threads in Request, and directs the errant thread to a wait if other threads are in request, allowing the signalled thread to operate.
     busy = true;

  if (busy){

      if ((position < floor) || ((position == floor) && !directionUp)) //If process should be picke up on upsweep
        upSweep -> Wait(lock, floor); //Priority wait based on floor
	
      else //Should be on downsweep
        downSweep->Wait(lock, 3 - floor);
   }

   busy = true; //Elevator is "delivering or picking up"

   position = floor; //Elevator moved to destination
   
   current --; //Thread has left request

   lock->Release();
}

void ElevatorManager::Release(){
   lock -> Acquire();

   busy = false; //Elevator has "completed pick up/deliver"

   //Determine if any threads are waiting on the down or up sweeps
   bool upWaiting; 
   bool downWaiting;
   upSweep-> ThreadWaiting(&upWaiting);
   downSweep -> ThreadWaiting(&downWaiting);

   if (directionUp){
      if (upWaiting)
         upSweep -> Signal(lock); //Get processes waiting on the up sweep if we're already going that way

      else{
         directionUp = false; //Otherwise go down and get those waiting on the downsweep
         downSweep -> Signal(lock);
      }
   }

   else{ //Mirror of above
      if (downWaiting)
         downSweep -> Signal(lock);

      else{
         directionUp = true;
         upSweep -> Signal(lock);
      }
   }

   lock -> Release();
}

