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
    newProc = Fork();
        if (newProc == 0) {
           
            char curChar;
            int count = 0;
            int argc = 0;
            int firstSpace = 0;
            int actualFirstSpace = 0;
            
            while ((curChar = buffer[count]) != '\0'){
                if (buffer[count] == ' ' && actualFirstSpace == 0)
                    actualFirstSpace = count;
                
                if (buffer[count] == ' ' && count < 60 && buffer[count + 1] != ' ' && buffer[count + 1] != '\0'){
                    argc ++; /*Includes the name of the executable, meaning it is one size too large. Therefore, enough for NULL ptr*/
                    firstSpace = count;
                }
                
                count ++;
            }
            
            prints("Argc:", ConsoleOutput);
            printd(argc, ConsoleOutput);
            prints("\n", ConsoleOutput);
            
            if (argc == 0){
                Exec(buffer, (char**) 0);
            
                prints("After exec\n", ConsoleOutput);
            }
            /*
            if (argc != 0){
                char* args[60];
                int start = firstSpace;
                int end = firstSpace + 1;
                int argNum = 0;
                int first = 0;
                
                prints("In else\n", ConsoleOutput);
                while ((curChar = buffer[end]) != '\0'){
                    if (buffer[end] == ' '){
                        if (buffer[end - 1] == ' '){
                            end ++;
                            start ++;
                        }
                        
                        else{
                            for (start = start++; start < end; start++)
                                args[argNum][start] = buffer[start];
                            
                            args[argNum][++start] = '\0';
                            prints(args[argNum], ConsoleOutput);
                            prints("\n", ConsoleOutput);
                            
                            argNum ++;
                            end ++;
                        }
                    }
                    
                    else
                        end ++;
                    
                }
                
                for (start = start++; start < end; start++)
                    args[argNum][start] = buffer[start];
                            
                args[argNum][++start] = '\0';
                
                args[argNum + 1] = (char*) 0;
                
                buffer[actualFirstSpace] = '\0';
                Exec(buffer, (char**) args);
            } */
            
            
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


