#include "syscall.h"

int
main()
{
    int i;
    int forkval = Fork();
    int openFile = Open("fork.out");

    if ( forkval == 0){
          prints("Child\n", openFile);
          prints("Child\n", openFile);
          Exit(0);
     }
     
     else{
	    prints("Parent\n", openFile);
        prints("\n", openFile);
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

