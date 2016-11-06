#ifdef CHANGED
#ifndef SLTIMER_H
#define SLTIMER_H

#include "copyright.h"
#include "utility.h"

// The following class defines a hardware timer. 
class SlicingTimer {
  public:
    SlicingTimer(VoidFunctionPtr timerHandler, int callArg);
				// Initialize the timer, to call the interrupt
				// handler "timerHandler" every time slice.
    ~SlicingTimer() {}

// Internal routines to the timer emulation -- DO NOT call these

    void TimerExpired();	// called internally when the hardware
				// timer generates an interrupt

  private:
    VoidFunctionPtr handler;	// timer interrupt handler 
    int arg;			// argument to pass to interrupt handler

};

#endif
#endif // TIMER_H
