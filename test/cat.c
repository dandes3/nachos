/* 
 *	cat.c
 *	Implement the cat system call just like in Linux
 *
 */ 
 
 #include "syscall.h" 


int main (int argc, char ** argv)
{
	int counter;
	char marker;
	OpenFileId pullFile;

	for (i = 1; i < argc; i++)
	{
		pullFile = Open(argv[i]);
		
		if (pullFile == -1) /* File does not exist or is locked */
		{
			prints("File could not be opened", ConsoleOutput);
			Exit(-1);
		}

		while(Read(&marker, 1, pullFile) == 1)
		{
			prints(&marker, ConsoleOutput);
		}
		Close(pullFile);
	}

	Exit(0);
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

