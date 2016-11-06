#ifdef CHANGED
/*
 *      cat.c 
 *      Implement the cat system call just like in Linux
 *
 */ 
 
 #include "syscall.h" 


int main (int argc, char **argv)
{
	int counter;
	char marker;
	OpenFileId pullFile;
        int i;

        /*prints("in cat\n", ConsoleOutput);*/
	for (i = 1; i < argc; i++)
	{
	/* 	prints("in loop\n", ConsoleOutput);*/
		pullFile = Open(argv[i]);
		/*prints("sucessfuly opened\n", ConsoleOutput);*/
		if (pullFile == -1) /* File does not exist or is locked */
		{
			prints("File could not be opened or does not exist\n", ConsoleOutput);
			/*break();*/
			Exit(-1);
		}

		while(Read(&marker, 1, pullFile) == 1)
		{
			prints(&marker, ConsoleOutput);
		}
		Close(pullFile);
	}
        /*prints("got to exit cleanly\n", ConsoleOutput);*/
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
#endif
