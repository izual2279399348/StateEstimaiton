#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <stdbool.h>
#include "State.h"
#ifndef ESTIMATION_H
#define ESTIMATION_H
automatonState *makeChannel(pTransition trans[],int tranSize,int N1);

automatonState *initialize(int tranSize);


typedef struct{
    int commonState[50][3];
    int stateNum;
}States, *pStates;



typedef struct{
    State currentRequired[3];
    State next[3];
    Event event;
}concurrentTransition, *pConcurrentTransition;


typedef struct{
    State current[3];
    State states[100][3];
    int stateNum;
    int transNum;
    pConcurrentTransition _transitionTable[100];
}concurrentAutomatonState,* pConcurrentAutomatonState;


struct observerNode{
    int stateNum;
    int numOfEvents;
    Event events[10];
    State state[5];
    State states[100][3];
    struct observerNode *next[10];
};

typedef struct observerNode Node;


//***************************************************OBTAIN THE FINAL CONCURRENT AUTOMATON MODEL****************************
concurrentAutomatonState *concurrent(pConcurrentAutomatonState auto_1, pAutomatonState auto_2);

//***************************************************CHECK WHETHER THE EVENT IS IN THE EVENT ARRAY**************************
bool checkIfNotIn(int arr[], int arrSize, int event);

//***************************************************CHECK WHETHER THE TRANSITION IS IN THE TRANSITION TABLE****************
bool checkIfInTransitionTable(pConcurrentTransition transitionCheck,pConcurrentTransition trans[],int tableSize);

//***************************************************CALCULATE THE NUMBER OF STATES WHICH IN THE TRANSITION TABLE***********
pStates checkStateSum(pConcurrentTransition transTable[],int tranSize);

//***************************************************DO THE IMPLEMENTATION OF THE SHUFFLE(CONCURRENT) ALGORITHM*************
concurrentAutomatonState *shuffle(pAutomatonState auto_1, pAutomatonState auto_2);

//***************************************************GERNERATE THE OBSERVER NODE *******************************************
Node *makeNode(State startState[], pConcurrentAutomatonState concurrentAuto);

//***************************************************CHECK IF STATE IN THE TRANSITON****************************************
bool checkIfTransitionRequired(pConcurrentTransition checkTransition,State stateCheck[]);

//***************************************************CHECK IF STATE IN THE NODE*********************************************
bool checkIfStateIn(int stateArray[],Node *check);

//***************************************************GERNERATE THE OBSERVER NODE TREE***************************************
void makeNextNode(Node *start, Node *previous, pConcurrentAutomatonState concurrentAuto);


void test(pConcurrentAutomatonState concurrentAuto);

#endif // STATE_H
