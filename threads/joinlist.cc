#include "joinlist.h"

JoinList::JoinList(){
    head = NULL;
    tail = NULL;
}


void 
JoinList::addNode(Thread *me, SpaceId child){
    
    JoinNode *newNode = new(std::nothrow) JoinNode;
    newNode -> parent = me;
    newNode -> childId = child;
    newNode -> permission = new(std::nothrow) Semaphore("foo", 0);
    newNode -> next = NULL;
    
    if (head == NULL){
        head = newNode;
        tail = newNode;
    }
    
    else{
        tail -> next = newNode;
        tail = newNode;
    }
}


void
JoinList::getNode(JoinNode* retVal, Thread *me, SpaceId child){
    
    JoinNode *cur;
    
    for (cur = head; cur != NULL; cur = cur -> next){
        printf("in for loop, me == %d, cur parent == %d\n", me, cur -> parent);
        printf("given childId: %d, cur child id: %d\n", child, cur -> childId);
        if (cur -> parent == me && cur -> childId == child)
            retVal = cur;
    }
    printf("Null ret\n");
}


void 
JoinList::deleteNode(JoinNode *garbage){
    
    JoinNode* cur;
    JoinNode* prev;
    JoinNode* temp;
    
    for(cur = head; cur != NULL; cur = cur -> next){
        if (cur == garbage){
            if (cur == head){
                if (cur == tail)
                    tail = NULL;
                
                temp = head;
                head = head -> next;
                delete temp;
            }
            
            for (prev = head; prev -> next != cur; prev = prev -> next);
            
            prev -> next = cur -> next;
            
            if (cur == tail)
                tail = prev;
            
            delete cur;
        }
    }
}
