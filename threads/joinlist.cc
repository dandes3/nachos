#include "joinlist.h"

#ifdef CHANGED

/* Fairly standard linked list initilization */
JoinList::JoinList(){
    head = NULL;
    tail = NULL;
}

/* Adds a node to the linked list. The parent thread ptr is initialized to "me"
 * and the childId is initialized to "child". Exit val is arbitrarily initialized to
 * 1337 to make it recognizable when a child has not yet exited.
 */
void 
JoinList::addNode(Thread *me, SpaceId child){
    
    JoinNode *newNode = new(std::nothrow) JoinNode; //Struc that makes up list
    newNode -> parent = me;
    newNode -> childId = child;
    newNode -> permission = new(std::nothrow) Semaphore("permission", 0); //Parent sleeps here on a Join, and is awoken on an Exit
    newNode -> next = NULL;
    newNode -> exitVal = 1337;
    
    //Normal linked list stuff    
    if (head == NULL){
        head = newNode;
        tail = newNode;
    }
    
    else{
        tail -> next = newNode;
        tail = newNode;
    }
}


/* Returns a JoinNode ptr that corresponds to the given parent and child. The parent requests this node
 * on a join in order to sleep on the permission semaphore. The child requests it to update the exit value and 
 * wake its parent on an exit
 */
JoinNode * 
JoinList::getNode(Thread *me, SpaceId child){
    
    JoinNode *cur;
    
    for (cur = head; cur != NULL; cur = cur -> next){
        if (cur -> parent == me && cur -> childId == child) //This is how a node is identified
            return cur;
    }
  
    return NULL;
}

/* Removes a node from the list. This is done by a parent after it has been awoken by its
 * child and retrieves the exit value from the JoinNode.
 */
void 
JoinList::deleteNode(JoinNode *garbage){
    
    JoinNode* cur;
    JoinNode* prev;
    JoinNode* temp;

    //Normal linked list stuff    
    for(cur = head; cur != NULL; cur = cur -> next){

        if (cur == garbage){
            if (cur == head){

                if (cur == tail)
                    tail = NULL;
                
                temp = head;
                head = head -> next;
                delete temp->permission;
                delete temp;
            }
            
            else{
                for (prev = head; prev -> next != cur; prev = prev -> next); //Efficiency traded for simplicity of code
            
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

/* Convienient function to print the contents of the join list. */
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

#endif
