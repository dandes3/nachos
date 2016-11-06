#include "syscall.h"

#ifdef CHANGED
int
main()
{
    SpaceId newProc;
    OpenFileId input = ConsoleInput;
    OpenFileId output = ConsoleOutput;
    char prompt[2], ch, buffer[60];
    int i, altout;
    int script = 0;
    int j;

    prompt[0] = '-';
    prompt[1] = '-';

    while( 1 )
    {
        Label:

        Write(prompt, 2, output);

        i = 0;
        
        do {
            j = Read(&buffer[i], 1, input);
            
            if (j == 0)
                Halt();
            
        } while( buffer[i++] != '\n');
        

        buffer[--i] = '\0';
      
        if (buffer[0] == '#'){
            script = 1; /*Used to determine if we are running a shell script*/
            goto Label; /*Don't tell any other professors or potential employers*/
        }
        
        if( i > 0 ) {
            if ((newProc = Fork()) == 0) { /* Child */
            
                int size = 0;
                int i = 0;
                char *args[60];
                int argc = 0;
                char *executable = 0;
        
                
                while ( buffer[size] != '\0'){ /*Replace all spaces with null bytes, making each arg "its own string" within the buffer*/
                    if (buffer[size] == ' ')
                        buffer[size] = '\0';
                    size ++;
                }
      
                while (i < size){
                    if (buffer[i] != '\0'){/*We have reached a character*/
                        if (executable == 0) /*First string is the executable name*/
                            executable = &buffer[i];
                        
                        args[argc] = &buffer[i]; /*First string is also the first argument, and subsequent args are the following arguments*/
                        argc ++;
                        
                        while (buffer[i] != '\0') /*Proceed until we reach the next character*/
                            i++;
                    }  
                    else
                        i++;  
                }

                argc --; /*We overcount by 1*/
                args[argc + 1] = (char *) 0; /*Convention, arg array is terminated by null ptr */

                if ((*(args[argc - 1]) == '>') && (*(args[argc - 1] + 1) == '\0')){  /*Output redirection*/
                    int fd;
                    
                    if (script) /*When a shell script is run, its stdout is closed to prevent extraneous output. It is reopened to make the dup work.*/
                        Open("/dev/ttyout");
                
                    /*Opens file, creates it if it doesn't exist*/
                    if ((fd = Open(args[argc])) == -1){
                        Create(args[argc]);
                        if ((fd = Open(args[argc])) == -1){
                            prints("Cannot open file\n", ConsoleOutput);
                            Exit(-1);
                        }
                    }                     

                    Close(1);
                    Dup(fd);
                    Close(fd);
                    args[argc - 1] = (char *) 0;
                } 
                

                if (script) /*Reopens stdout so it can be used by the script executable*/
                    Open("/dev/ttyout");
                
                    
                Exec(executable, (char**) args);
                Close(1);

                altout = Open("/dev/ttyout"); /*Ensures stdout is open for a script/redirected output as well*/            
                prints("Error, executable does not exist\n", altout);
                Close(altout);
                Halt();
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

#endif
