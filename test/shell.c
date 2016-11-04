#include "syscall.h"


int
main()
{
    SpaceId newProc;
    OpenFileId input = ConsoleInput;
    OpenFileId output = ConsoleOutput;
    char prompt[2], ch, buffer[60];
    int i;

    prompt[0] = '-';
    prompt[1] = '-';

    while( 1 )
    {
    Write(prompt, 2, output);

    i = 0;
    
    do {
    
        Read(&buffer[i], 1, input); 

    } while( buffer[i++] != '\n' );

    buffer[--i] = '\0';

    if( i > 0 ) {
    /*newProc = Fork();*/
        if ((newProc = Fork()) == 0) { /* Child */
           
            char curChar;
            char prevChar;
            int count = 0;
            int argc = 0;
            int beforeFirstArg = -1; /* index of the space before the first argument */
            int firstSpace = 0; /* index of first space in buffer */
            
            
            while ((curChar = buffer[count]) != '\0'){
                if (buffer[count] == ' ' && firstSpace == 0) /* Sets the position of the first space in the buffer */
                    firstSpace = count;
                
                if (buffer[count] == ' ' && count < 60 && buffer[count + 1] != ' ' && buffer[count + 1] != '\0'){ /* If current character is the first space after a word */
                    argc ++;
                    if (beforeFirstArg == -1)
                        beforeFirstArg = count;
                }
                
                count ++;
            }
            
            prints("Argc:", ConsoleOutput);
            printd(argc, ConsoleOutput);
            prints("\n", ConsoleOutput);
            
            prints("beforeFirstArg is ", ConsoleOutput);
            printd(beforeFirstArg, ConsoleOutput);
            prints("\n", ConsoleOutput);
            
            if (argc == 0){ /* No arguments */
                prints("No arguments\n");
                Exec(buffer, (char**) 0);
            
                prints("After exec\n", ConsoleOutput);
            }
            
            if (argc != 0){ /* Yes arguments */
                
                char* args[60]; /* array of arguments */
                int start = beforeFirstArg;
                int end = beforeFirstArg + 1;
                int argNum = 0;
                int first = 0;
                char temp[2];
                int pos = 0;
                prints("yes arguments\n", ConsoleOutput);
                while ((curChar = buffer[end]) != '\0'){
                    prints("end is ", ConsoleOutput);
                    printd(end, ConsoleOutput);
                    prints("\n", ConsoleOutput);
                    
                    prints("curChar is ", ConsoleOutput);
                    temp[0] = curChar;
                    temp[1] = '\0';
                    prints(temp, ConsoleOutput);
                    prints("\n", ConsoleOutput);
                    
                    if (curChar == ' '){
                        prints("curChar == ' '\n", ConsoleOutput);
                        prevChar = buffer[end-1];
                        if (prevChar == ' '){ /* iterate past space */
                            prints("prevChar == ' '\n", ConsoleOutput);
                            end ++;
                            start ++;
                        }
                        
                        else{ /* start is the space before arg, end is the space after arg */
                            prints("prevChar != ' '\n", ConsoleOutput);
                            pos = 0;
                            for (start = ++start; start < end; start++)
                                args[argNum][pos++] = buffer[start];

                            args[argNum][++start] = '\0';
                            prints("arg is ", ConsoleOutput);
                            prints(args[argNum], ConsoleOutput);
                            prints("\n", ConsoleOutput);
                            
                            argNum ++;
                            end ++;
                        }
                    }
                    
                    else
                        end ++;
                    
                }
                pos = 0;
                for (start = start++; start < end; start++)
                    args[argNum][pos++] = buffer[start];
                            
                args[argNum][++start] = '\0';
                prints("arg is ", ConsoleOutput);
                prints(args[argNum], ConsoleOutput);
                prints("\n", ConsoleOutput);
                
                args[argNum + 1] = (char*) 0;
                
                buffer[firstSpace] = '\0';
                Exec(buffer, (char**) args);
            } 
            
            
        }
    else Join(newProc);
    }
    }
}

prints(s,file)
char *s;
OpenFileId file;

{
  while (*s != '\0') {
    Write(s,1,file);
    s++;
  }
}

/* Print an integer "n" on open file descriptor "file". */

printd(n,file)
int n;
OpenFileId file;

{

  int i;
  char c;

  if (n < 0) {
    Write("-",1,file);
    n = -n;
  }
  if ((i = n/10) != 0)
    printd(i,file);
  c = (char) (n % 10) + '0';
  Write(&c,1,file);
}


