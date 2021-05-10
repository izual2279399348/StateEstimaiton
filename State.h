#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#ifndef STATE_H
#define STATE_H
typedef int State;
typedef int Event;

#define STATENUM 3
#define STATE1 0
#define STATE2 1
#define STATE3 2

#define EVENTS 4
#define EVENT1 0
#define EVENT2 1
#define EVENT3 2
#define EVENT4 3

typedef struct{
    State currentRequired;
    State next;
    Event event;
}transition, *pTransition;

void onAction(State mystate,Event event);

typedef struct{
    State current;
    State states[100];
    int stateNum;
    int transNum;
    pTransition _transitionTable[100];
}automatonState,* pAutomatonState;


extern transition t1;
extern transition t2;
extern transition t3;
extern transition t4;
extern pTransition transitionTable[];
extern pTransition channel1[3];
extern pTransition channel2[1];


State step(pAutomatonState machine, Event event);

#endif // STATE_H
