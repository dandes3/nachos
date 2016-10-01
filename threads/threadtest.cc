// threadtest.cc 
//	Simple test case for the threads assignment.
//
//	Create two threads, and have them context switch
//	back and forth between themselves by calling Thread::Yield, 
//	to illustratethe inner workings of the thread system.
//
// Copyright (c) 1992-1993 The Regents of the University of California.
// All rights reserved.  See copyright.h for copyright notice and limitation 
// of liability and disclaimer of warranty provisions.

#include "copyright.h"
#include "system.h"
#include <vector>
#include "synch.h"

//----------------------------------------------------------------------
// SimpleThread
// 	Loop 5 times, yielding the CPU to another ready thread 
//	each iteration.
//
//	"which" is simply a number identifying the thread, for debugging
//	purposes.
//----------------------------------------------------------------------

char *buffer; // The buffer where characters will be placed and removed from
Lock *lock; // A lock for accessing the buffer
Lock *bridgeLock; // A lock for accessing the bridge
Lock *consumerLock; // A lock for consumers to access charsToBeConsumed

Condition *notEmpty; // Checks if the buffer is not empty
Condition *notFull; // Checks if the buffer is not full
Condition *bridgeNotFull; // Checks if the bridge is full;
Condition *bridgeDirection; // Changes the direction of traffic when bridge is empty

int buffSize; // The input size for the buffer. Passed in through the command line
int pos; // position in the buffer to either insert or remove a char from
int charsToBeConsumed; // Number of characters that producers will put in.
        // Equal to strlen("HELLO WORLD") * number of producers.
        // Consumers will remove exactly this amount of characters from the buffer
        // in case there is an unequal number of producers and consumers.
int numCharsConsumed; // Number of characters already consumed. Goal is to get this
        // value to be charsToBeConsumed to indicate that consumers are done.
int carsOnBridge; // Number of cars on the bridge
int currentDirection; // Current direction of cars on bridge
int nameCount = 0;

void Insert(char c); // Prototype for a producer function to insert c into the buffer
void Remove(); // Prototype for a consumer function to remove the last character from the buffer
void Producer(int n); // The function that gets forked per producer thread
void Consumer(int n); // The function that gets forked per consumer thread
void OneVehicle(int direction);
void ArriveBridge(int direction, int id);
void CrossBridge(int direction, int id);
void ExitBridge(int direction, int id);

void Test(struct fourStruct *fs);
struct fourStruct{
    int myDir;
    int myNum;
};

void SimpleThread(int which){
    int num;
    
    for (num = 0; num < 5; num++) {
	printf("*** thread %d looped %d times\n", which, num);
        currentThread->Yield();
    }
}

//----------------------------------------------------------------------
// ThreadTest
// 	Set up a ping-pong between two threads, by forking a thread 
//	to call SimpleThread, and then calling SimpleThread ourselves.
//----------------------------------------------------------------------
void ThreadTest(){
    DEBUG('t', "Entering SimpleTest");

    Thread *t = new(std::nothrow) Thread("forked thread");

    t->Fork(SimpleThread, 1);
    SimpleThread(0);
}

//----------------------------------------------------------------------
//  Producer
// 	Inputs one character from "HELLO WORLD" one character at a time
//----------------------------------------------------------------------
void Producer(int n){
    //DEBUG('t', "producer\n");
    
    char *string = "HELLO WORLD";
    for (int i = 0; i < strlen(string); i++){
        Insert(string[i]);
    }
    
    
}

//----------------------------------------------------------------------
//  Consumer
// 	Removes one character from "HELLO WORLD" one character at a time.
//  Since the number of consumers might be less than or greater than them
//  number of producers, then a Consumer knows to stop removing when
//  the same number of characters have been consumed as those produced
//----------------------------------------------------------------------
void Consumer(int n){
    //DEBUG('t', "consumer\n");
    
    char *string = "HELLO WORLD";
    
    int numConsumed; // local data
    
    consumerLock->Acquire();
    numConsumed = numCharsConsumed; // the Remove function modifies the global
            // numCharsConsumed. So to read it, we lock it
    consumerLock->Release();
    
    while(numConsumed < charsToBeConsumed){
        Remove();
        consumerLock->Acquire();
        numConsumed = numCharsConsumed;
        consumerLock->Release();
    }
}

//----------------------------------------------------------------------
//  Insert
// 	Inserts one character, c, to the end of the buffer. If the buffer is full, it
//  must wait. The end of the buffer is marked by pos. After an insertion, we signal 
//  that the buffer is not empty
//----------------------------------------------------------------------
void Insert(char c){
    DEBUG('t', "insert\n");
    
    lock->Acquire();

    while (strlen(buffer) >= buffSize) { // wait if the buffer is full
        notFull->Wait(lock);
    }
    buffer[pos++] = c;
    //DEBUG('t', "insert entered %c\n", c);
    //DEBUG('t', "insert's buffer is %s\n", buffer);
    
    printf("%s\n", buffer);
    //printf("%s\t%d\n", buffer, strlen(buffer));
    
    notEmpty->Signal(lock);
    
    lock->Release();
}

//----------------------------------------------------------------------
//  Remove
// 	Removes one character from the end of the buffer.  If the buffer is empty, it
//  must wait. The end of the buffer is marked by pos. After a deletion, we signal
//  that the buffer is not full. Remove also increases the number of characters
//  consumed.
//----------------------------------------------------------------------
void Remove(){
    DEBUG('t', "remove\n");
    
    lock->Acquire();
    
    while (strlen(buffer) <= 0){ // wait if the buffer is empty
        notEmpty->Wait(lock);
    }
    
    DEBUG('t', "remove's buffer is %s\n", buffer);
    char c = buffer[pos - 1];
    buffer[pos-1] = '\0';
    pos--;
    //DEBUG('t', "remove's count is %d\n", pos);
    //DEBUG('t', "remove got %c\n", c);
    
    printf("%s\n", buffer);
    //printf("%s\t%d\n", buffer, strlen(buffer));
    
    notFull->Signal(lock);
    lock->Release();
    
    consumerLock->Acquire();
    numCharsConsumed++;
    consumerLock->Release();
    
}

//----------------------------------------------------------------------
//  Two
// 	The producer and consumer problem. Here, all the global variables such as the
//  buffer, locks, conditions, and counters are initialized here.
//  A fork is called per producer and consumer thread with dummy arguments of 1.
//----------------------------------------------------------------------
void Two(int numPros, int numCons, int sizeOfBuffer){
    buffSize = sizeOfBuffer;
    charsToBeConsumed = numPros * 11; // 11 characters in "HELLO WORLD"
    printf("Two\n");
    //DEBUG('t', "Entering Two\n");
    //DEBUG('t', "numPro is %d\n", numPros);
    //DEBUG('t', "numCon is %d\n", numCons);
    //DEBUG('t', "buffSize is %d\n", buffSize);
    //DEBUG('t', "charsToBeConsumed is %d\n", charsToBeConsumed);
    
    buffer = new char[buffSize];
    lock = new(std::nothrow) Lock("Lock");
    consumerLock = new(std::nothrow) Lock("consumerLock");
    pos = 0;
    
    bzero(buffer, buffSize);
    
    notEmpty = new(std::nothrow) Condition("notEmpty");
    notFull = new(std::nothrow) Condition("notFull");
    
    // fork off numPros number of producers
    for (int i = 0; i < numPros; i++){
        char temp[10];
        sprintf(temp, "%d", i);
        char name[100];

        strcpy(name,"producer");
        strcat(name, temp); // name should be producer0, producer1, etc...
        
        Thread *proThread = new Thread(name);
        //DEBUG('t', "name is %s\n", name);
        //DEBUG('t', "producer name is %s\n", proThread->getName());
        proThread->Fork(Producer, 1); // arg 1 is a dummy argument
    }
    // fork off numCons number of consumers
    for (int i = 0; i < numPros; i++){
        char temp[10];
        sprintf(temp, "%d", i);
        char name[100];

        strcpy(name,"consumer");
        strcat(name, temp); // name should be producer0, producer1, etc...
        
        Thread *conThread = new Thread(name);
        //DEBUG('t', "name is %s\n", name);
        conThread->Fork(Consumer, 1);
    }
        
}



void Three(){
    
}


void OneVehicle(int direction){

    int arrival_delay = 1+(int) (100000.0*rand()/(RAND_MAX+1.0));
    int crossing_time = 1+(int) (100000.0*rand()/(RAND_MAX+1.0));
    
    int id;
    //nameLock->Acquire();
    //id = nameCount;
    //nameLock->Release();
    
    
    for (int i=0; i < arrival_delay; i++){
        interrupt->SetLevel(IntOff);
        interrupt->SetLevel(IntOn);
    }
    ArriveBridge(direction, id);
    
    CrossBridge(direction, id);
    for (int i=0; i < crossing_time; i++){
        interrupt->SetLevel(IntOff);
        interrupt->SetLevel(IntOn);
    }
    
    ExitBridge(direction, id);
}

void ArriveBridge(int direction, int id){

    char *dirName = new char[6];
    char *curName = new char[6];
    bzero(dirName, 6);
    bzero(dirName, 6);
    
    bridgeLock->Acquire();
    
    if (direction == 0){
        strcpy(dirName, "NORTH");
    }
    else{
        strcpy(dirName, "SOUTH");
    }
    if (currentDirection == 0){
        strcpy(curName, "NORTH");
    }
    else{
        strcpy(curName, "SOUTH");
    }
        
    printf("Car going %s and traffic is going %s tried to ARRIVE. There were %d cars on the bridge.\n", dirName, curName, carsOnBridge);
    
    while (carsOnBridge == 3 || currentDirection != direction){
        if (carsOnBridge == 3){
            printf("Car going %s and traffic is going %s has to WAIT because the bridge is FULL.\n", dirName, curName);
            bridgeNotFull->Wait(bridgeLock);
            printf("The bridge is NO LONGER full.\n", dirName, curName);
        }
        
        if (currentDirection != direction){
            printf("Car going %s and traffic is going %s has to WAIT because of ONCOMING traffic.\n", dirName, curName);
            bridgeDirection->Wait(bridgeLock);
            currentDirection = direction;
            printf("Since there no cars on the bridge. Switching traffic flow to %s\n", dirName);
            strcpy(curName, dirName);
        }
    }
    
    carsOnBridge++;
    
    if (carsOnBridge != 3){
        bridgeNotFull->Signal(bridgeLock);
    }
    printf("Car going %s and traffic is going %s successfully ARRIVED. There are now  %d cars on the bridge.\n", dirName, curName, carsOnBridge);
    bridgeLock->Release();
    
    
}

void CrossBridge(int direction, int id){
    
    char *dirName = new char[6];
    char *curName = new char[6];
    bzero(dirName, 6);
    bzero(dirName, 6);
    
    if (direction == 0){
        strcpy(dirName, "NORTH");
    }
    else{
        strcpy(dirName, "SOUTH");
    }

    printf("Car going %s is now CROSSING the bridge\n", dirName);
}

void ExitBridge(int direction, int id){
    
    
    char *dirName = new char[6];
    char *curName = new char[6];
    bzero(dirName, 6);
    bzero(dirName, 6);
    
    bridgeLock->Acquire();
    
    if (direction == 0){
        strcpy(dirName, "NORTH");
    }
    else{
        strcpy(dirName, "SOUTH");
    }
    if (currentDirection == 0){
        strcpy(curName, "NORTH");
    }
    else{
        strcpy(curName, "SOUTH");
    }
    printf("Car going %s and traffic is going %s began to EXIT. There were %d cars on the bridge.\n", dirName, curName, carsOnBridge);
    
    carsOnBridge--;
    bridgeNotFull->Signal(bridgeLock);
    if (carsOnBridge == 0){
        bridgeDirection->Signal(bridgeLock);
    }
    
    printf("Car going %s and traffic is going %s successfully EXITED. There are now %d cars on the bridge.\n", dirName, curName, carsOnBridge);
    
    
    bridgeLock->Release();
    
    
}

void Four(int numCars){
    DEBUG('t', "four\n");
    
    bridgeLock = new Lock("bridgeLock");
    
    bridgeNotFull = new Condition("bridgeFull");
    bridgeDirection = new Condition("bridgeDirection");
    
    carsOnBridge = 0;
    
    for (int i = 0; i < numCars; i++){
        int direction = rand() % 2;
        if (i == 0){
            currentDirection = direction;
            if (currentDirection == 0){
                printf("FIRST car to arrive will be going NORTH, so setting up traffic to go NORTH\n");
            }
            else{
                printf("FIRST car to arrive will be going SOUTH, so setting up traffic to go SOUTH\n");
            }
        }
 
        Thread *vehicle = new Thread("Vehicle");
        vehicle->Fork(OneVehicle, direction);
    }
}
