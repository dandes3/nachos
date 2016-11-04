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
    newNode -> exitVal = 1337;
    
    if (head == NULL){
        head = newNode;
        tail = newNode;
    }
    
    else{
        tail -> next = newNode;
        tail = newNode;
    }
}


JoinNode * 
JoinList::getNode(Thread *me, SpaceId child){
    
    JoinNode *cur;
    
    for (cur = head; cur != NULL; cur = cur -> next){
        //printf("in for loop, me == %d, cur parent == %d\n", me, cur -> parent);
       // printf("given childId: %d, cur child id: %d\n", child, cur -> childId);
        if (cur -> parent == me && cur -> childId == child)
            return cur;
    }
    printf("Null ret\n");
    return NULL;
}


void 
JoinList::deleteNode(JoinNode *garbage){
    
    JoinNode* cur;
    JoinNode* prev;
    JoinNode* temp;
    
    for(cur = head; cur != NULL; cur = cur -> next){
        //fprintf(stderr, "About to segfault? addr of cur is %x and addr of head is %x and addr of tail is %x\n", cur, head, tail);
        if (cur -> exitVal == NULL)
            //fprintf(stderr, "cur is null\n");
        
        //fprintf(stderr, "cur exit val is: %d\n", cur -> exitVal);
        if (cur == garbage){
            if (cur == head){
                if (cur == tail)
                    tail = NULL;
                
                temp = head;
                head = head -> next;
                //elete temp->permission;
                //delete temp;
            }
            
            else{
                for (prev = head; prev -> next != cur; prev = prev -> next);
            
                prev -> next = cur -> next;
            
                if (cur == tail)
                    tail = prev;
                delete cur->permission;
                delete cur;
            }
            
            break;
        }
    }
}

void
JoinList::print(){
	
	JoinNode * cur;

	for (cur = head; cur != NULL; cur = cur -> next){
       printf("Parent: %s           ", cur -> parent -> getName());
	}
	printf("\n");
   for (cur = head; cur != NULL; cur = cur -> next){
       printf("ExitVal: %d    -->    ", cur -> exitVal);
    }
	printf("\n");
   for (cur = head; cur != NULL; cur = cur -> next){
       printf("ChildID: %d                 ", cur ->  childId);
    }

	printf("\n");
}

