#ifdef CHANGED
#include "copyright.h"
#include "slicingTimer.h"
#include "system.h"

// dummy function because C++ does not allow pointers to member functions
static void SlicingTimerHandler(int arg)
{ SlicingTimer *p = (SlicingTimer *)arg; p->TimerExpired(); }

//----------------------------------------------------------------------
// SlicingTimer::SlicingTimer
//      Initialize a hardware timer device.  Save the place to call
//	on each interrupt, and then arrange for the timer to start
//	generating interrupts. Schedules an interrupt every 100 ticks.
//
//      "timerHandler" is the interrupt handler for the timer device.
//		It is called with interrupts disabled every time the
//		the timer expires.
//      "callArg" is the parameter to be passed to the interrupt handler.
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
// SlicingTimer::TimerExpired
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
#endif
