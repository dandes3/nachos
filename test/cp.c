/* 
 *	cp.c
 *	Implement the cp system call just like in Linux
 *
 */ 
 
 #include "syscall.h" 


int main (int argc, char ** argv)
{
	/* int counter; */
	char marker;
	OpenFileId pullFile;
	OpenFileId pushFile;

	if (argc != 3)
	{
		prints("Incorrect amount of arguments, try again");
		Exit(-1);

		else
		{
			pull = Open(argv[1]); /* Open the file to be copied */

			if (pull == -1)
			{
				prints("File could not be opened", ConsoleOutput);
			    Exit(-1);
			}

			else
			{
				push = Open(argv[2]); /* Open the file to be made */
			}
		}
	}


/* Fancy print function */
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
