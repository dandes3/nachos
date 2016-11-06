#ifndef JOINLIST_H
#define JOINLIST_H

#ifdef CHANGED
#include "copyright.h"
#include "synch.h"
#include "syscall.h"
#include "thread.h"

/* Struct that makes up each node in the join list. The parent and
 * childId fields are used to identify the node such that the permission
 * semaphore and exitval fields can be used appropriately.
 */
typedef struct  JoinNode{
    Thread *parent;
    SpaceId childId;
    int exitVal;
    Semaphore *permission;
    JoinNode *next;
} JoinNode;


class JoinList{
    
    private:
        JoinNode *head;
        JoinNode *tail;
        
    public:
        JoinList();
        void addNode(Thread *me, SpaceId child);
        JoinNode *getNode(Thread *me, SpaceId child);
        void deleteNode(JoinNode *garbage);
        void print();        
    
};
#endif
#endif
