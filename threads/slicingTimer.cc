// of liability and disclaimer of warranty provisions.

#include "copyright.h"
#include "slicingTimer.h"
#include "system.h"

// dummy function because C++ does not allow pointers to member functions
static void SlicingTimerHandler(int arg)
{ SlicingTimer *p = (SlicingTimer *)arg; p->TimerExpired(); }

//----------------------------------------------------------------------
// Timer::Timer
//      Initialize a hardware timer device.  Save the place to call
//	on each interrupt, and then arrange for the timer to start
//	generating interrupts.
//
//      "timerHandler" is the interrupt handler for the timer device.
//		It is called with interrupts disabled every time the
//		the timer expires.
//      "callArg" is the parameter to be passed to the interrupt handler.
//      "doRandom" -- if true, arrange for the interrupts to occur
//		at random, instead of fixed, intervals.
//----------------------------------------------------------------------

SlicingTimer::SlicingTimer(VoidFunctionPtr timerHandler, int callArg)
{
    handler = timerHandler;
    arg = callArg; 

    // schedule the first interrupt from the timer device
    interrupt->Schedule(SlicingTimerHandler, (int) this, 100, 
		TimerInt); 
}

//----------------------------------------------------------------------
// Timer::TimerExpired
//      Routine to simulate the interrupt generated by the hardware 
//	timer device.  Schedule the next interrupt, and invoke the
//	interrupt handler.
//----------------------------------------------------------------------
void 
SlicingTimer::TimerExpired() 
{
    // schedule the next timer device interrupt
    interrupt->Schedule(SlicingTimerHandler, (int) this, 100, 
		TimerInt);

    // invoke the Nachos interrupt handler for this device
    (*handler)(arg);
}

/*
//----------------------------------------------------------------------
// Timer::TimeOfNextInterrupt
//      Return when the hardware timer device will next cause an interrupt.
//	If randomize is turned on, make it a (pseudo-)random delay.
//----------------------------------------------------------------------

int 
Timer::TimeOfNextInterrupt() 
{
	return 100;  
}
*/