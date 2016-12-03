/* cowwrite.c 
 *
 * Process is small enough to fit in primary memory.
 * Fork()s a child process which writes data space.
 *
 * Used as a disk i/o benchmark to evaluate cow.
 *
 */

#include "syscall.h"

#define VECSIZE 800
#define LOOPS   1

int vector[VECSIZE];                        /* 25 pages of data */

int
main()
{
  register int i, j;
  SpaceId cowkid;

  prints("Starting COW\n", ConsoleOutput);

  for (i = 0; i < VECSIZE; i++) vector[i] = i;

  prints("Initialization complete in parent\n", ConsoleOutput);

  if ((cowkid = Fork()) == 0) {

    /* Here is the code of the child process. */

    for (j = 0; j < LOOPS; j++)
      for (i = 0; i < VECSIZE; i++)
        vector[i] = i;

    /* Exit value semi-randomly picked. */
    
    Exit(vector[17]);
  }

  i = Join(cowkid);

  prints("Parent off Join with a value of ", ConsoleOutput);
  printd(i, ConsoleOutput);
  prints("\n", ConsoleOutput);
  Halt();
}

/* Print a null-terminated string "s" on open file descriptor "file". */

prints(s,file)
char *s;
OpenFileId file;

{
  int count = 0;
  char *p;

  p = s;
  while (*p++ != '\0') count++;
  Write(s, count, file);

}


/* Print an integer "n" on open file descriptor "file". */

printd(n,file)
int n;
OpenFileId file;

{

  int i, pos=0, divisor=1000000000, d, zflag=1;
  char c;
  char buffer[11];

  if (n < 0) {
    buffer[pos++] = '-';
    n = -n;
  }

  if (n == 0) {
    Write("0",1,file);
    return;
  }

  for (i=0; i<10; i++) {
    d = n / divisor; n = n % divisor;
    if (d == 0) {
      if (!zflag) buffer[pos++] =  (char) (d % 10) + '0';
    } else {
      zflag = 0;
      buffer[pos++] =  (char) (d % 10) + '0';
    }
    divisor = divisor/10;
  }
  Write(buffer,pos,file);
}
