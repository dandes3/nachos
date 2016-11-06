#include <syscall.h>

int main(){
    char *fileName = "atomicFile";
    int joinval;
    SpaceId kid;
    int fd;
    int i,j;
    Create(fileName);
    fd = Open(fileName);
    if ((kid = Fork()) == 0){ /*Child*/
        
        for (i = 0; i < 3; i++){
            Write("ABBABB", 6, fd);
        }
        Exit(5);
    }
    else{ /*Parent*/
        for (j = 0; j < 7; j++){
            Write("CDDDDCDDDD", 10, fd);
        }
        joinval = Join(kid);
        Halt(9);
    }

}
