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
#define MAX_CARS 3
//----------------------------------------------------------------------
// SimpleThread
// 	Loop 5 times, yielding the CPU to another ready thread 
//	each iteration.
//
//	"which" is simply a number identifying the thread, for debugging
//	purposes.
//----------------------------------------------------------------------

char *buffer; // The buffer where characters will be placed and removed from
Lock *bufferLock; // A lock for accessing the buffer
Lock *bridgeLock; // A lock for accessing the bridge
Lock *consumerLock; // A lock for consumers to access charsToBeConsumed

Condition *bufferNotEmpty; // Indicates that the buffer is not empty
Condition *bufferNotFull; // Indicates that the buffer is not full
Condition *bridgeNotFull; // Indicates that the bridge is not full;
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
int trafficDirection; // Current direction of cars on bridge

int nameCount = 1; // a vehicle id for ensuring the code is correct

void Insert(char c); // Prototype for a producer function to insert c into the buffer
void Remove(); // Prototype for a consumer function to remove the last character from the buffer
void Producer(int n); // The function that gets forked per producer thread
void Consumer(int n); // The function that gets forked per consumer thread
void OneVehicle(int myDirection); // Controls when a vehicle arrives,
    // crosses, or exits the bridge
int ArriveBridge(int myDirection); // Determines whether the car can cross
void CrossBridge(int myDirection, int id); // Crosses the bridge
void ExitBridge(int myDirection, int id); // Exits the bridge and lets others know

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
            // numCharsConsumed. So to initalize it, we have get the global copy.
            // This accounts for the case where there are many consumers
            // and few producers. 
    consumerLock->Release();
    
    while(numConsumed < charsToBeConsumed){ // We have to make sure
            // that we don't consume more characters than producers
            // will input into the buffer. That is, one consumer can still
            // clear the buffer regardless of the number of producers
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
    
    bufferLock->Acquire();

    while (strlen(buffer) >= buffSize) { // wait if the buffer is full
        bufferNotFull->Wait(bufferLock);
    }
    buffer[pos++] = c;
    //DEBUG('t', "insert entered %c\n", c);
    //DEBUG('t', "insert's buffer is %s\n", buffer);
    
    printf("%s\n", buffer);
    //printf("%s\t%d\n", buffer, strlen(buffer));
    
    bufferNotEmpty->Signal(bufferLock); // Signal that the buffer is not empty
    
    bufferLock->Release();
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
    
    bufferLock->Acquire();
    
    while (strlen(buffer) <= 0){ // wait if the buffer is empty
        bufferNotEmpty->Wait(bufferLock);
    }
    
    DEBUG('t', "remove's buffer is %s\n", buffer);
    char c = buffer[pos - 1];
    buffer[pos-1] = '\0';
    pos--;
    //DEBUG('t', "remove's count is %d\n", pos);
    //DEBUG('t', "remove got %c\n", c);
    
    printf("%s\n", buffer);
    
    bufferNotFull->Signal(bufferLock); // Signal that the buffer is not full
    bufferLock->Release();
    
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
    
    buffer = new char[buffSize]; // initalize buffer to input buffSize
    
    // instantiate locks
    bufferLock = new(std::nothrow) Lock("bufferLock");
    consumerLock = new(std::nothrow) Lock("consumerLock");
    
    pos = 0; //First producer will input into the 0th element of the buffer
    
    bzero(buffer, buffSize); //clear the buffer
    
    //instantiate conditions
    bufferNotEmpty = new(std::nothrow) Condition("bufferNotEmpty");
    bufferNotFull = new(std::nothrow) Condition("bufferNotFull");
    
    // fork off numPros number of producers
    for (int i = 0; i < numPros; i++){
        Thread *proThread = new Thread("Producer");
        proThread->Fork(Producer, 1); // arg 1 is a dummy argument
    }
    
    // fork off numCons number of consumers
    for (int i = 0; i < numPros; i++){
        Thread *conThread = new Thread("Consumer");
        conThread->Fork(Consumer, 1);
    }
}



void Three(){
    
}

//----------------------------------------------------------------------
//  OneVehicle
// 	Controls how much time passes before the vehicle arrives to the bridge.
//  Controls how much time it takes for a vehicle to cross the bridge.
//  Calls the ArriveBride, CrossBridge, and ExitBridge functions at the appropriate
//  times.
//----------------------------------------------------------------------
void OneVehicle(int myDirection){

    int arrival_delay = 1+(int) (100000.0*rand()/(RAND_MAX+1.0));
    int crossing_time = 1+(int) (100000.0*rand()/(RAND_MAX+1.0));
    
    int id; // A local copy of the id for the vehicle. 
        // This helps a reader verify the correctness of the code by
        // allowing them to see which vehicle is performing a specific action
    
    // Delay before arriving
    for (int i=0; i < arrival_delay; i++){
        interrupt->SetLevel(IntOff);
        interrupt->SetLevel(IntOn);
    }
    
    id = ArriveBridge(myDirection); // ArriveBridge modifies the global variant
        // of id, nameCount, and returns it as to keep it local to the thread.
    
    CrossBridge(myDirection, id);
    
    // Simulate travel time for crossing a bridge
    for (int i=0; i < crossing_time; i++){
        interrupt->SetLevel(IntOff);
        interrupt->SetLevel(IntOn);
    }
    
    ExitBridge(myDirection, id);
}

//----------------------------------------------------------------------
//  ArriveBridge
// 	Determine if the bridge is full or if there is oncoming traffic. If so, wait.
//  If not (or if condition is signaled), then increase the number of cars on the
//  bridge. Return the id of this car.
//----------------------------------------------------------------------
int ArriveBridge(int myDirection){
    
    // instantiate two empty strings to represent myDirection and trafficDirection.
    // For reading convenience, a direction of 0 corresponds to "NORTH" and 1 to 
    // "SOUTH"
    char *myDirectionName = new char[6]; // strlen("NORTH") = strlen("SOUTH") = 6
    char *trafficDirectionName = new char[6];
    
    // Clear the strings
    bzero(myDirectionName, 6);
    bzero(myDirectionName, 6);
    
    // Acquire the lock
    bridgeLock->Acquire();
    
    int id = nameCount++; // this is the only place that modifies or 
    // reads nameCount
    
    // set myDirectionName to "NORTH" if myDirection is 0
    // set myDirectionName to "SOUTH" if myDirection is 1
    if (myDirection == 0){
        strcpy(myDirectionName, "NORTH");
    }
    else{
        strcpy(myDirectionName, "SOUTH");
    }
    
    // set trafficDirectionName to "NORTH" if trafficDirection is 0
    // set trafficDirectionName to "SOUTH" if trafficDirection is 1
    if (trafficDirection == 0){
        strcpy(trafficDirectionName, "NORTH");
    }
    else{
        strcpy(trafficDirectionName, "SOUTH");
    }
        
    printf("Car %d going %s and traffic is going %s tried to ARRIVE. There were %d cars on the bridge.\n", id, myDirectionName, trafficDirectionName, carsOnBridge);
    
    while (carsOnBridge == MAX_CARS || trafficDirection != myDirection){ // wait if
            // bridge is full or if there is oncoming traffic
        if (carsOnBridge == MAX_CARS){
            printf("Car %d going %s and traffic is going %s has to WAIT because the bridge is FULL.\n", id, myDirectionName, trafficDirectionName);
            bridgeNotFull->Wait(bridgeLock); //Wait until the bridge is not full
            printf("The bridge is NO LONGER full.\n", myDirectionName, trafficDirectionName);
        }
        
        if (trafficDirection != myDirection){
            printf("Car %d going %s and traffic is going %s has to WAIT because of ONCOMING traffic.\n", id, myDirectionName, trafficDirectionName);
            bridgeDirection->Wait(bridgeLock); // wait until there 
                // is no oncoming traffic.
                // This only happens when there are no cars on the bridge
            trafficDirection = myDirection; // since the bridge is now empty
                // then the direction of traffic is the direction that
                // this vehicle moves in.
            printf("Since there no cars on the bridge. Switching traffic flow to %s\n", myDirectionName);
            strcpy(trafficDirectionName, myDirectionName);
        }
    }
    
    carsOnBridge++; // increment the number of cars on the bridge
    
    if (carsOnBridge != MAX_CARS){
        bridgeNotFull->Signal(bridgeLock); // Signal that the bridge is not full
    }
    
    printf("Car %d going %s and traffic is going %s successfully ARRIVED. There are now  %d cars on the bridge.\n", id, myDirectionName, trafficDirectionName, carsOnBridge);
    
    bridgeLock->Release(); // release the lock
    return id;
    
}

//----------------------------------------------------------------------
//  CrossBridge
// 	Merely states that a certain car is now crossing the bridge
//----------------------------------------------------------------------
void CrossBridge(int myDirection, int id){
    
    char *myDirectionName = new char[6];
    char *trafficDirectionName = new char[6];
    bzero(myDirectionName, 6);
    bzero(myDirectionName, 6);
    
    if (myDirection == 0){
        strcpy(myDirectionName, "NORTH");
    }
    else{
        strcpy(myDirectionName, "SOUTH");
    }

    printf("Car %d going %s is now CROSSING the bridge\n", id, myDirectionName);
}

//----------------------------------------------------------------------
//  ExitBridge
// 	Decrement the number of cars on the bridge. If there are no cars, 
//  the direction of traffic is subject to change, so signal those waiting for 
//  oncoming traffic to clear.
//----------------------------------------------------------------------
void ExitBridge(int myDirection, int id){
    
    char *myDirectionName = new char[6];
    char *trafficDirectionName = new char[6];
    bzero(myDirectionName, 6);
    bzero(myDirectionName, 6);
    
    bridgeLock->Acquire(); // Acquire the lock
    
    if (myDirection == 0){
        strcpy(myDirectionName, "NORTH");
    }
    else{
        strcpy(myDirectionName, "SOUTH");
    }
    if (trafficDirection == 0){
        strcpy(trafficDirectionName, "NORTH");
    }
    else{
        strcpy(trafficDirectionName, "SOUTH");
    }
    printf("Car %d going %s and traffic is going %s began to EXIT. There were %d cars on the bridge.\n", id, myDirectionName, trafficDirectionName, carsOnBridge);
    
    carsOnBridge--; // decrement the number of cars on the bridge
    
    bridgeNotFull->Signal(bridgeLock);
    if (carsOnBridge == 0){
        bridgeDirection->Signal(bridgeLock);
    }
    
    printf("Car %d going %s and traffic is going %s successfully EXITED. There are now %d cars on the bridge.\n", id, myDirectionName, trafficDirectionName, carsOnBridge);
    
    
    bridgeLock->Release();
    
    
}

void Four(int numCars){
    DEBUG('t', "four\n");
    
    bridgeLock = new Lock("bridgeLock");
    
    bridgeNotFull = new Condition("bridgeFull");
    bridgeDirection = new Condition("bridgeDirection");
    
    carsOnBridge = 0;
    
    for (int i = 0; i < numCars; i++){
        int myDirection = rand() % 2;
        if (i == 0){
            trafficDirection = myDirection;
            if (trafficDirection == 0){
                printf("FIRST car to arrive will be going NORTH, so setting up traffic to go NORTH\n");
            }
            else{
                printf("FIRST car to arrive will be going SOUTH, so setting up traffic to go SOUTH\n");
            }
        }
 
        Thread *vehicle = new Thread("Vehicle");
        vehicle->Fork(OneVehicle, myDirection);
    }
}
