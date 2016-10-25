#include "syscall.h"

int
main()
{
    int i;

    if ( Fork() == 0){
	for (i = 0; i < 5; i ++)
          prints("Child", ConsoleOutput);
        prints("\n", ConsoleOutput);
     }
     
     else{
	for (i = 0; i < 5; i ++)
	  prints("Parent", ConsoleOutput);
        prints("\n", ConsoleOutput);
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

