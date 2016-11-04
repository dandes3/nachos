#include "syscall.h"


int
main()
{
    SpaceId newProc;
    OpenFileId input = ConsoleInput;
    OpenFileId output = ConsoleOutput;
    char prompt[2], ch, buffer[60];
    int i;
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
            
            if (j == 0){
                Halt();
            }
            
            /*if ((int) buffer[i] == -1){
                Halt();
            }*/
        } while( buffer[i++] != '\n' );
        
        buffer[--i] = '\0';
      
        if (buffer[0] == '#'){
            script = 1;
            goto Label;
        }
        
        if( i > 0 ) {
            if ((newProc = Fork()) == 0) { /* Child */
            
                int size = 0;
                int i = 0;
                char *args[60];
                int argc = 0;
                char *executable = 0;

                while ( buffer[size] != '\0'){
                    if (buffer[size] == ' ')
                        buffer[size] = '\0';
                    size ++;
                }
      
                while (i < size){
                    if (buffer[i] != '\0'){
                        if (executable == 0)
                            executable = &buffer[i];
                        
                        args[argc] = &buffer[i];
                        argc ++;
                        
                        while (buffer[i] != '\0')
                            i++;
                    }  
                    else
                        i++;  
                }

                argc --;
                args[argc + 1] = (char *) 0;  
                
                if ((*(args[argc - 1]) == '>') && (*(args[argc - 1] + 1) == '\0')){ 
                    int fd;
                    prints("In dup stuff\n", ConsoleOutput);
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
                 
                else if (script)
                    Open("/dev/ttyout");
                    
                Exec(executable, (char**) args);
                prints("Error, executable does not exist\n", ConsoleOutput);
                Exit(-1);
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


