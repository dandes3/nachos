#include "syscall.h"

int
main()
{
    int i;
    int forkval = Fork();
    int openFile = Open("fork.out");

    if ( forkval == 0){
          prints("Child\n", ConsoleOutput);
          prints("Child\n", ConsoleOutput);
          Exit(0);
     }
     
     else{
	    prints("Parent\n", ConsoleOutput);
        prints("\n", ConsoleOutput);
        Join(forkval);
     }
    

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

