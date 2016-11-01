
#include "syscall.h"

int
main()
{

  SpaceId kid;
  int joinval;

  prints("Great Grand PARENT exists\n", ConsoleOutput);
  kid = Fork();
  if (kid != 0) {
    prints("Great Grand PARENT after fork; kid pid is ", ConsoleOutput);
    printd((int)kid, ConsoleOutput);
    prints("\n", ConsoleOutput);
    
    joinval = Join(kid);
    
    prints("Great Grand PARENT off Join with value of ", ConsoleOutput);
    printd(joinval, ConsoleOutput);
    prints("\n", ConsoleOutput);

   /* Halt();*/
  /* not reached */
  } else {
           int join2;
    	   SpaceId grandKid = Fork();
            
 	   if (grandKid != 0){	
    	   prints("Grand PARENT after fork; grandKid pid is ", ConsoleOutput);
  	      printd((int)grandKid, ConsoleOutput);
         prints("\n", ConsoleOutput);

    	   join2 = Join(grandKid);

    	   prints("Grand PARENT off Join with value of ", ConsoleOutput);
   	   printd(join2, ConsoleOutput);
    	   prints("\n", ConsoleOutput);

         prints("KID about to Exit()\n", ConsoleOutput);
         Exit(10);
      }

      else{
         int join3;
         SpaceId greatGrandKid = Fork();

         if (greatGrandKid != 0){
            prints("PARENT after fork; greatGrandKid pid is ", ConsoleOutput);
            printd((int)greatGrandKid, ConsoleOutput);
            prints("\n", ConsoleOutput);

            join3 = Join(greatGrandKid);

            prints("PARENT off Join with value of ", ConsoleOutput);
            printd(join3, ConsoleOutput);
            prints("\n", ConsoleOutput);

            prints("Grand KID about to Exit()\n", ConsoleOutput);
            Exit(100);
         }

         else{
            prints("Great Grand KID about to Exit()\n", ConsoleOutput);
            Exit(1000);

         }
      }
   }
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





