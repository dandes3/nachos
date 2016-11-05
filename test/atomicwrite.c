#include <syscall.h>

int main(){
    char *fileName = "atomicFile";
    int joinval;
    SpaceId kid;
    int fdChild, fdParent;
    int i,j;
    if ((kid = Fork()) == 0){ /*Child*/
        fdChild = Open(fileName);
        for (i = 0; i < 3; i++){
            Write("CCCCC", 5, fdChild);
        }
        Exit(5);
    }
    else{ /*Parent*/
        fdParent = Open(fileName);
        for (j = 0; j < 7; j++){
            Write("PPPPPPPPP", 9, fdParent);
        }
        Exit(9);
    }

}
