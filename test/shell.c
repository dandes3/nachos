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
          
            if (argc == 0){ /* No arguments */
                Exec(buffer, (char**) 0);
            
                prints("After exec\n", ConsoleOutput);
            }
            
            if (argc != 0){ /* Yes arguments */
                
                char *args[60]; /* array of arguments */
                char curArg[60];
                int start = beforeFirstArg;
                int end = beforeFirstArg + 1;
                int argNum = 0;
                int pos = 0;
                int startPos = 0;
       
                while ((curChar = buffer[end]) != '\0'){ 
                    if (curChar == ' '){
                   
                        if (buffer[end - 1] == ' '){  
                            end ++;
                            start ++;
                        }
                     
                        else{ 
                         /* start is the space before arg, end is the space after arg */
                            startPos = 0;
                            for (start = ++start; start < end; start++){
                                curArg[pos++] = buffer[start];
                                startPos ++;
                            }
                 
                            curArg[++pos] = '\0';
                            args[argNum] = curArg + (pos - startPos - 1);
                                
                            argNum ++;
                            end ++;
                            pos ++;
                        }
                    }
                    
                    else
                        end ++;
                    
                }

                startPos = 0;
                for (start = ++start; start < end; start++){
                    curArg[pos++] = buffer[start];
                    startPos ++;
                 }                
                curArg[++pos] = '\0';      
                args[argNum] =  curArg + (pos - startPos - 1);

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


