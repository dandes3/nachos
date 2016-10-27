#ifndef JOINLIST_H
#define JOINLIST_H

#include "copyright.h"
#include "synch.h"
#include "syscall.h"
#include "thread.h"

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
        void getNode(JoinNode *retVal,Thread *me, SpaceId child);
        void deleteNode(JoinNode *garbage);
        
    
};
#endif
