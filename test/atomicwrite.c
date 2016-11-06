#include <syscall.h>

int main(){
    char *fileName = "atomicFile";
    int joinval;
    SpaceId kid;
    int fd;
    int i,j;
    fd = Open(fileName);
    if ((kid = Fork()) == 0){ /*Child*/
        
        for (i = 0; i < 3; i++){
            Write("%d CCCCC\n", 8, fd);
        }
        Exit(5);
    }
    else{ /*Parent*/
        for (j = 0; j < 7; j++){
            Write("%d PPPPPPPPP\n", 12, fd);
        }
        joinval = Join(kid);
        Halt(9);
    }

}
