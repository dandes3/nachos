#ifndef ELMANAGER_H
#define ELMANAGER_H

#include "synch.h"

class ElevatorManager{
   public:
      ElevatorManager();
      ~ElevatorManager();

      void Request(bool dropOff, int pid, int floor);
      void Arrived();

   private:
      int position, current;
      bool directionUp, busy;
      Condition *upSweep, *downSweep;
      Lock *lock;
};
 

#endif 
