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
		prints("Incorrect amount of arguments, try again", ConsoleOutput);
		Exit(-1);
	}
		else
		{
			pullFile = Open(argv[1]); /* Open the file to be copied */

			if (pullFile == -1)
			{
				prints("File could not be opened or does not exist\n", ConsoleOutput);
			    Exit(-1);
			}

			else
			{
				pushFile = Open(argv[2]); /* Open the file to be made */

				if(pushFile == -1)
				{
					Create(argv[2]);
					pushFile = (Open(argv[2]));
				}

				while(Read(&marker, 1, pullFile) == 1)
				{
					Write(&marker, 1, pushFile);
				}

				Close(pullFile);
				Close(pushFile);
			}
		}

		Exit(0); /* Successful */ 
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
