/* vmtoobig.c
 *
 * Parent forks off kid which is too big. Should survive.
 *
 */

#include "syscall.h"

int
main()
{

  SpaceId kid;
  int joinval;
  char *args[2];

  args[0] = "huge";
  args[1] = (char *)0;

  if ((kid = Fork()) == 0) {
    Exec("huge",args);
    prints("Exec() failed in PARENT\n");
    Exit(16);
  }
  
  prints("PARENT after fork/exec; kid pid is ", ConsoleOutput);
  printd((int)kid, ConsoleOutput);
  prints("\n", ConsoleOutput);

  prints("PARENT about to Join kid\n", ConsoleOutput);
  joinval = Join(kid);
  prints("PARENT off Join with value of ", ConsoleOutput);
  printd(joinval, ConsoleOutput);
  prints("\n", ConsoleOutput);

  Halt();
  /* not reached */
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
