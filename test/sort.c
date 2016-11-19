/* sort.c 
 *    Test program to sort a large number of integers.
 *
 *    Intention is to stress virtual memory system.
 *
 *    Ideally, we could read the unsorted array off of the file system,
 *	and store the result back to the file system!
 */

#include "syscall.h"

int A[1024];	/* size of physical memory; with code, we'll run out of space!*/

int
main()
{
    int i, j, tmp;

    /* first initialize the array, in reverse sorted order */
    for (i = 0; i < 1024; i++)		
        A[i] = 1024 - i;

    Write("Initialized\n",12, ConsoleOutput);
    /* then sort! */
    for (j = 1; j < 1024; j++) {       /* Brutal insertion sort. */
        Write("j=",2, ConsoleOutput);
        printd(j, ConsoleOutput);
        Write("\n", 1, ConsoleOutput);

        tmp = A[j];
        i = j - 1;
        do {
          if (tmp >= A[i]) break;
          A[i+1] = A[i];
          i--;
        } while (i>=-1);
        A[i+1] = tmp;
    }
    printd(A[0],ConsoleOutput);		/* and then we're done -- should be 1! */
    Write("\n", 1, ConsoleOutput);
    Exit(0);
}

/* Print an integer "n" on open file descriptor "file". */

printd(n,file)
int n;
OpenFileId file;

{

  int i;
  char c;

  if (n < 0) {
    Write("-",1,file);
    n = -n;
  }
  if ((i = n/10) != 0)
    printd(i,file);
  c = (char) (n % 10) + '0';
  Write(&c,1,file);
}
