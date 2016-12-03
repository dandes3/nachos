/* kid6.c
 *
 * Child in the toobig system. Unitialized data
 * too big to fit on the system disk.
 *
 */

#include "syscall.h"

int huge[1000000];

int
main()
{
  int i, j;
  
  for (i=0; i++; i<10000) j++;
  /* loop to delay kid initially; hope parent gets to Join and sleeps */
  Exit(6);
  /* Should not get past here */
  Write("KID after Exit()!\n", 18, ConsoleOutput);
  Halt();
    /* not reached */
}
