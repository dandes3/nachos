#include <syscall.h>

int main(){
    char *fileName = "atomicFile";
    int joinval;
    SpaceId kid;
    int childFd, parentFd;
    int i,j;
    if ((kid = Fork()) == 0){ /*Child*/
        childFd = Open(fileName);
        for (i = 0; i < 3; i++){
            Write("CCCCC", 5, childFd);
        }
        Exit(5);
    }
    else{ /*Parent*/
        for (j = 0; j < 7; j++){
	    parentFd = Open(fileName);
            Write("PPPPPPPPP", 9, parentFd);
        }
        Exit(9);
    }

}
