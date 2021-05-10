#include "Estimation.h"






//***************************************************GERNERATE THE CHANNEL AUTOMATON***********************************************
automatonState *makeChannel(pTransition trans[],int tranSize,int N){
    pAutomatonState channelAuto;
    channelAuto = (automatonState *)malloc(sizeof(automatonState));

    channelAuto->current = 0;
    channelAuto->stateNum = N*tranSize+1;
    channelAuto->transNum = N*tranSize*2;


    pTransition theChannel[N*tranSize*2];
    for(int i = 0;i<N*tranSize;i++){

        theChannel[2*i] = (pTransition)malloc(sizeof(transition));
        theChannel[2*i+1] = (pTransition)malloc(sizeof(transition));

        if(i==0){
            theChannel[2*i]->currentRequired = 0;
            theChannel[2*i]->next = trans[0]->event+1;
            theChannel[2*i]->event = trans[0]->event+1;
        }else if(i%2==1){
            if(tranSize == 1 && N == 2){
                theChannel[2*i]->event = trans[0]->event+1;
                theChannel[2*i]->currentRequired = 4;
                theChannel[2*i]->next = theChannel[2*i]->currentRequired + theChannel[2*i]->event;
            }else{
                theChannel[2*i]->event = i+1;
                theChannel[2*i]->currentRequired = theChannel[i-1]->currentRequired;
                theChannel[2*i]->next = theChannel[i-1]->next+1;
            }
        }else if(i%2==0){
            if(tranSize == 1 && N == 2){
                theChannel[2*i]->event = trans[0]->event+1;
                theChannel[2*i]->currentRequired = theChannel[i]->next;
                theChannel[2*i]->next = theChannel[2*i]->currentRequired + theChannel[2*i]->event;
            }else{
                theChannel[2*i]->currentRequired = theChannel[i]->currentRequired;
                theChannel[2*i]->next = theChannel[i]->next+1;
                theChannel[2*i]->event = i+1;
            }
        }

        theChannel[2*i+1]->currentRequired = theChannel[2*i]->next;
        theChannel[2*i+1]->next = theChannel[2*i]->currentRequired;
        theChannel[2*i+1]->event = (theChannel[2*i]->event)*-1;

    }
    channelAuto->states[0] = 0;
    for(int n=0;n<N*tranSize;n++){
        channelAuto->states[n+1] = theChannel[2*n]->next;
    }

    for(int i=0;i<N*tranSize*2;i++){
        channelAuto->_transitionTable[i] = theChannel[i];
    }



    return channelAuto;
}


//***************************************************INITIALIZE THE MODEL AUTOMATON****************************************************
automatonState *initialize(int tranSize){
    pAutomatonState automaton;
    automaton = (automatonState *)malloc(sizeof(automatonState));
    automaton->current = 1;
    automaton->transNum = tranSize;
    for(int i=0;i<tranSize;i++){
        automaton->_transitionTable[i] = transitionTable[i];
    }
    for(int i=0;i<tranSize;i++){
        automaton->_transitionTable[i]->currentRequired = automaton->_transitionTable[i]->currentRequired+1;
        automaton->_transitionTable[i]->event = automaton->_transitionTable[i]->event+1;
        automaton->_transitionTable[i]->next = automaton->_transitionTable[i]->next+1;
    }
    return automaton;
}



//***************************************************OBTAIN THE FINAL CONCURRENT AUTOMATON MODEL***************************************

concurrentAutomatonState *concurrent(pConcurrentAutomatonState auto_1, pAutomatonState auto_2){
    pConcurrentAutomatonState concurrent;
    concurrent = (pConcurrentAutomatonState)malloc(sizeof(concurrentAutomatonState));

    concurrent->current[0] = auto_1->current[0];
    concurrent->current[1] = auto_1->current[1];
    concurrent->current[2] = auto_2->current;

    int commonEvents[50];
    int checkState = 0;
    int counter = 0;
    int arrayFlag = 0;
    pConcurrentTransition transitionFinal[100];

    //Find common event
    for(int i=0;i<auto_1->transNum;i++){
        for(int j=0;j<auto_2->transNum;j++){
            if(auto_1->_transitionTable[i]->event==auto_2->_transitionTable[j]->event){
                    transitionFinal[counter] = (pConcurrentTransition)malloc(sizeof(concurrentTransition));

                    transitionFinal[counter]->currentRequired[0] = auto_1->_transitionTable[i]->currentRequired[0];
                    transitionFinal[counter]->currentRequired[1] = auto_1->_transitionTable[i]->currentRequired[1];
                    transitionFinal[counter]->currentRequired[2] = auto_2->_transitionTable[j]->currentRequired;

                    transitionFinal[counter]->event = auto_1->_transitionTable[i]->event;

                    transitionFinal[counter]->next[0] = auto_1->_transitionTable[i]->next[0];
                    transitionFinal[counter]->next[1] = auto_1->_transitionTable[i]->next[1];
                    transitionFinal[counter]->next[2] = auto_2->_transitionTable[j]->next;

                    counter++;

                    commonEvents[arrayFlag] = auto_1->_transitionTable[i]->event;
                    arrayFlag++;

            }
        }
    }


    pStates states;

    //calculate the number of the states now in the transition table
    states=checkStateSum(transitionFinal,counter);

    concurrentTransition checkTransition = {{0,0,0},{0,0,0},0};
    while(1){
        for(int i=0;i<states->stateNum;i++){
                //find the non-common event in the automaton 1

            for(int n=0;n<auto_1->transNum;n++){
                if(checkIfNotIn(commonEvents,arrayFlag,auto_1->_transitionTable[n]->event)){
                    if(auto_1->_transitionTable[n]->currentRequired[0] == states->commonState[i][0]&&
                       auto_1->_transitionTable[n]->currentRequired[1] == states->commonState[i][1]){


                        checkTransition.currentRequired[0] = states->commonState[i][0];
                        checkTransition.currentRequired[1] = states->commonState[i][1];
                        checkTransition.currentRequired[2] = states->commonState[i][2];

                        checkTransition.event = auto_1->_transitionTable[n]->event;

                        checkTransition.next[0] = auto_1->_transitionTable[n]->next[0];
                        checkTransition.next[1] = auto_1->_transitionTable[n]->next[1];
                        checkTransition.next[2] = states->commonState[i][2];

                        if(!checkIfInTransitionTable(&checkTransition,transitionFinal,counter)){
                            checkState = 1;
                            transitionFinal[counter] = (pConcurrentTransition)malloc(sizeof(concurrentTransition));

                            transitionFinal[counter]->currentRequired[0] = checkTransition.currentRequired[0];
                            transitionFinal[counter]->currentRequired[1] = checkTransition.currentRequired[1];
                            transitionFinal[counter]->currentRequired[2] = checkTransition.currentRequired[2];

                            transitionFinal[counter]->event = checkTransition.event;

                            transitionFinal[counter]->next[0] = checkTransition.next[0];
                            transitionFinal[counter]->next[1] = checkTransition.next[1];
                            transitionFinal[counter]->next[2] = checkTransition.next[2];

                            counter++;


                        }

                    }


                }

            }
            //calculate the number of the states now in the transition table
            free(states);
            states=NULL;

            states=checkStateSum(transitionFinal,counter);
            //find the non-common event in the automaton 2
            for(int n=0;n<auto_2->transNum;n++){
                if(checkIfNotIn(commonEvents,arrayFlag,auto_2->_transitionTable[n]->event)){
                    if(auto_2->_transitionTable[n]->currentRequired == states->commonState[i][2]){
                        checkTransition.currentRequired[0] = states->commonState[i][0];
                        checkTransition.currentRequired[1] = states->commonState[i][1];
                        checkTransition.currentRequired[2] = states->commonState[i][2];

                        checkTransition.event = auto_2->_transitionTable[n]->event;

                        checkTransition.next[0] = states->commonState[i][0];
                        checkTransition.next[1] = states->commonState[i][1];
                        checkTransition.next[2] = auto_2->_transitionTable[n]->next;


                        if(!checkIfInTransitionTable(&checkTransition,transitionFinal,counter)){
                            checkState = 1;
                            transitionFinal[counter] = (pConcurrentTransition)malloc(sizeof(concurrentTransition));

                            transitionFinal[counter]->currentRequired[0] = checkTransition.currentRequired[0];
                            transitionFinal[counter]->currentRequired[1] = checkTransition.currentRequired[1];
                            transitionFinal[counter]->currentRequired[2] = checkTransition.currentRequired[2];

                            transitionFinal[counter]->event = checkTransition.event;

                            transitionFinal[counter]->next[0] = checkTransition.next[0];
                            transitionFinal[counter]->next[1] = checkTransition.next[1];
                            transitionFinal[counter]->next[2] = checkTransition.next[2];

                            counter++;

                        }


                    }
                }

            }

            //calculate the number of the states now in the transition table
            free(states);
            states=NULL;

            states=checkStateSum(transitionFinal,counter);


        }





        if(checkState==0){
            break;
        }
        checkState = 0;

    }

    for(int n=0;n<counter;n++){
            concurrent->_transitionTable[n] = transitionFinal[n];
    }
    concurrent->transNum = counter;
    concurrent->stateNum = states->stateNum;


    return concurrent;
}


//***************************************************CHECK WHETHER THE EVENT IS IN THE EVENT ARRAY************************************
bool checkIfNotIn(int arr[], int arrSize, int event){
    for(int i=0;i<arrSize;i++){
        if(arr[i]==event){
            return 0;
        }
    }
    return 1;
}




//***************************************************CHECK WHETHER THE TRANSITION IS IN THE TRANSITION TABLE**************************
bool checkIfInTransitionTable(pConcurrentTransition transitionCheck,pConcurrentTransition trans[],int tableSize){
    for(int i=0;i<tableSize;i++){
        if(transitionCheck->currentRequired[0]==trans[i]->currentRequired[0]){
            if(transitionCheck->currentRequired[1]==trans[i]->currentRequired[1]){
                if(transitionCheck->currentRequired[2]==trans[i]->currentRequired[2]){
                    if(transitionCheck->event==trans[i]->event){
                        if(transitionCheck->next[0]==trans[i]->next[0]){
                            if(transitionCheck->next[1]==trans[i]->next[1]){
                                if(transitionCheck->next[2]==trans[i]->next[2]){
                                    return 1;
                                }
                            }
                        }
                    }

                }
            }
        }
    }
    return 0;

}

//***************************************************CALCULATE THE NUMBER OF STATES WHICH IN THE TRANSITION TABLE*********************
pStates checkStateSum(pConcurrentTransition transTable[],int tranSize){
    pStates states;
    states = (pStates)malloc(sizeof(States));

    int tag1 = 0;
    int tag2 = 0;
    int counter = 0;

    for(int i=0;i<tranSize;i++){
        for(int j=0;j<counter;j++){
            if(transTable[i]->currentRequired[0]==states->commonState[j][0]&&
               transTable[i]->currentRequired[1]==states->commonState[j][1]&&
               transTable[i]->currentRequired[2]==states->commonState[j][2]){
                tag1 = 1;
            }

            if(transTable[i]->next[0]==states->commonState[j][0]&&
               transTable[i]->next[1]==states->commonState[j][1]&&
               transTable[i]->next[2]==states->commonState[j][2]){
                tag2 = 1;
            }

        }

        if(tag1==0){
            states->commonState[counter][0] = transTable[i]->currentRequired[0];
            states->commonState[counter][1] = transTable[i]->currentRequired[1];
            states->commonState[counter][2] = transTable[i]->currentRequired[2];
            counter++;
        }

        if(tag2==0){
            states->commonState[counter][0] = transTable[i]->next[0];
            states->commonState[counter][1] = transTable[i]->next[1];
            states->commonState[counter][2] = transTable[i]->next[2];
            counter++;
        }
        tag1=0;
        tag2=0;
    }

    states->stateNum = counter;

    return states;


}


//***************************************************DO THE IMPLEMENTATION OF THE SHUFFLE(CONCURRENT) ALGORITHM********************************
concurrentAutomatonState *shuffle(pAutomatonState auto_1, pAutomatonState auto_2){

    pConcurrentAutomatonState concurrent_1;
    concurrent_1 = (pConcurrentAutomatonState)malloc(sizeof(concurrentAutomatonState));

    concurrent_1->current[0] = auto_1->current;
    concurrent_1->current[1] = auto_2->current;
    concurrent_1->current[2] = 0;

    int commonEvents[10];
    int arrayFlag = 0;
    int transitionFlag = 0;
    pConcurrentTransition transitionShuffle[100];

    //Find the common event
    for(int i=0;i<auto_1->transNum;i++){
        for(int j=0;j<auto_2->transNum;j++){
            if(auto_1->_transitionTable[i]->event==auto_2->_transitionTable[j]->event){
                transitionShuffle[transitionFlag] = (pConcurrentTransition)malloc(sizeof(concurrentTransition));

                transitionShuffle[transitionFlag]->currentRequired[0] = auto_1->_transitionTable[i]->currentRequired;
                transitionShuffle[transitionFlag]->currentRequired[1] = auto_2->_transitionTable[j]->currentRequired;
                transitionShuffle[transitionFlag]->currentRequired[2] = 0;

                transitionShuffle[transitionFlag]->event = auto_1->_transitionTable[i]->event;

                transitionShuffle[transitionFlag]->next[0] = auto_1->_transitionTable[i]->next;
                transitionShuffle[transitionFlag]->next[1] = auto_2->_transitionTable[j]->next;
                transitionShuffle[transitionFlag]->next[2] = 0;

                transitionFlag++;


                commonEvents[arrayFlag] = auto_1->_transitionTable[i]->event;
                arrayFlag++;
            }
        }
    }

    int checkState = 0;
    pStates states;

    //calculate the number of the states now in the transition table
    states=checkStateSum(transitionShuffle,transitionFlag);

    pConcurrentTransition checkTransition;
    checkTransition = (pConcurrentTransition)malloc(sizeof(concurrentTransition));

    while(1){
        for(int i=0;i<states->stateNum;i++){
                //find the non-common event in the automaton 1
            for(int n=0;n<auto_1->transNum;n++){
                if(checkIfNotIn(commonEvents,arrayFlag,auto_1->_transitionTable[n]->event)){
                    if(auto_1->_transitionTable[n]->currentRequired == states->commonState[i][0]){
                        checkTransition->currentRequired[0] = states->commonState[i][0];
                        checkTransition->currentRequired[1] = states->commonState[i][1];
                        checkTransition->currentRequired[2] = 0;

                        checkTransition->event = auto_1->_transitionTable[n]->event;

                        checkTransition->next[0] = auto_1->_transitionTable[n]->next;
                        checkTransition->next[1] = states->commonState[i][1];
                        checkTransition->next[2] = 0;

                        if(!checkIfInTransitionTable(checkTransition,transitionShuffle,transitionFlag)){
                            checkState = 1;
                            transitionShuffle[transitionFlag] = (pConcurrentTransition)malloc(sizeof(concurrentTransition));

                            transitionShuffle[transitionFlag]->currentRequired[0] = checkTransition->currentRequired[0];
                            transitionShuffle[transitionFlag]->currentRequired[1] = checkTransition->currentRequired[1];
                            transitionShuffle[transitionFlag]->currentRequired[2] = checkTransition->currentRequired[2];

                            transitionShuffle[transitionFlag]->event = checkTransition->event;

                            transitionShuffle[transitionFlag]->next[0] = checkTransition->next[0];
                            transitionShuffle[transitionFlag]->next[1] = checkTransition->next[1];
                            transitionShuffle[transitionFlag]->next[2] = checkTransition->next[2];

                            transitionFlag++;


                        }

                    }


                }

            }
            //calculate the number of the states now in the transition table
            free(states);
            states=NULL;

            states=checkStateSum(transitionShuffle,transitionFlag);
            //find the non-common event in the automaton 2

            for(int n=0;n<auto_2->transNum;n++){
                if(checkIfNotIn(commonEvents,arrayFlag,auto_2->_transitionTable[n]->event)){
                    if(auto_2->_transitionTable[n]->currentRequired == states->commonState[i][1]){
                        checkTransition->currentRequired[0] = states->commonState[i][0];
                        checkTransition->currentRequired[1] = states->commonState[i][1];
                        checkTransition->currentRequired[2] = 0;

                        checkTransition->event = auto_2->_transitionTable[n]->event;

                        checkTransition->next[0] = states->commonState[i][0];
                        checkTransition->next[1] = auto_2->_transitionTable[n]->next;
                        checkTransition->next[2] = 0;


                        if(!checkIfInTransitionTable(checkTransition,transitionShuffle,transitionFlag)){

                            checkState = 1;
                            transitionShuffle[transitionFlag] = (pConcurrentTransition)malloc(sizeof(concurrentTransition));

                            transitionShuffle[transitionFlag]->currentRequired[0] = checkTransition->currentRequired[0];
                            transitionShuffle[transitionFlag]->currentRequired[1] = checkTransition->currentRequired[1];
                            transitionShuffle[transitionFlag]->currentRequired[2] = checkTransition->currentRequired[2];

                            transitionShuffle[transitionFlag]->event = checkTransition->event;

                            transitionShuffle[transitionFlag]->next[0] = checkTransition->next[0];
                            transitionShuffle[transitionFlag]->next[1] = checkTransition->next[1];
                            transitionShuffle[transitionFlag]->next[2] = checkTransition->next[2];

                            transitionFlag++;
                        }

                    }


                }

            }

            //calculate the number of the states now in the transition table
            free(states);
            states=NULL;

            states=checkStateSum(transitionShuffle,transitionFlag);


        }


        if(checkState==0){
            break;
        }
        checkState = 0;

    }
    free(checkTransition);
    checkTransition = NULL;

    //copy the transition table to the automaton
    for(int num=0;num<transitionFlag;num++){
        concurrent_1->_transitionTable[num] = transitionShuffle[num];
    }

    concurrent_1->transNum = transitionFlag;

    return concurrent_1;

}





//***************************************************GERNERATE THE OBSERVER NODE ***************************************
Node *makeNode(State startState[], pConcurrentAutomatonState concurrentAuto){

    Node *node = (Node *)malloc(sizeof(Node));


    int current_1 = startState[0];
    int current_2 = startState[1];
    int current_3 = startState[2];

    int arrayFlag = 0;
    int positonOfStack = 0;

    State statesMixed[10][3];



    statesMixed[arrayFlag][0] = current_1;
    statesMixed[arrayFlag][1] = current_2;
    statesMixed[arrayFlag][2] = current_3;
    arrayFlag++;
    //Traversing nodes in the form of queue, that is, first in first out
    while(arrayFlag>positonOfStack){
        positonOfStack++;
        for(int i=0;i<concurrentAuto->transNum;i++){
            if(concurrentAuto->_transitionTable[i]->currentRequired[0] == current_1){
                if(concurrentAuto->_transitionTable[i]->currentRequired[1] == current_2){
                    if(concurrentAuto->_transitionTable[i]->currentRequired[2] == current_3){
                        if(concurrentAuto->_transitionTable[i]->event>0){
                            statesMixed[arrayFlag][0] = concurrentAuto->_transitionTable[i]->next[0];
                            statesMixed[arrayFlag][1] = concurrentAuto->_transitionTable[i]->next[1];
                            statesMixed[arrayFlag][2] = concurrentAuto->_transitionTable[i]->next[2];
                            arrayFlag++;

                        }

                    }
                }
            }

        }
        if(arrayFlag>positonOfStack){
            current_1 = statesMixed[positonOfStack][0];
            current_2 = statesMixed[positonOfStack][1];
            current_3 = statesMixed[positonOfStack][2];

        }
    }

    int check = 0;
    int pState = 0;
    //Add the state to the node
    for(int n=0;n<arrayFlag;n++){
        node->states[n][0] = statesMixed[n][0];
        node->states[n][1] = statesMixed[n][1];
        node->states[n][2] = statesMixed[n][2];

        for(int m=0;m<n;m++){
            if(statesMixed[n][0]==statesMixed[m][0]){
                    check = 1;
            }
        }
        if(check == 0){
            node->state[pState] = statesMixed[n][0];
            pState++;
            check = 0;
        }else if(check == 1){
            check = 0;
        }
    }

    int pEvent = 0;
    //Add the event which could occurred to the node
    for(int n=0;n<arrayFlag;n++){
        for(int i=0;i<concurrentAuto->transNum;i++){
            if(concurrentAuto->_transitionTable[i]->currentRequired[0] == statesMixed[n][0]){
                if(concurrentAuto->_transitionTable[i]->currentRequired[1] == statesMixed[n][1]){
                    if(concurrentAuto->_transitionTable[i]->currentRequired[2] == statesMixed[n][2]){
                        if(concurrentAuto->_transitionTable[i]->event<0){
                                if(pEvent==0||checkIfNotIn(node->events,pEvent,concurrentAuto->_transitionTable[i]->event)){
                                        node->events[pEvent] = concurrentAuto->_transitionTable[i]->event;
                                        node->next[pEvent] = NULL;
                                        pEvent++;
                                }
                        }

                    }
                }
            }

        }

    }
    node->stateNum = arrayFlag;
    node->numOfEvents = pEvent;
    return node;

}



//***************************************************CHECK IF STATE IN THE TRANSITON****************************************
bool checkIfTransitionRequired(pConcurrentTransition checkTransition,State stateCheck[]){
    if(checkTransition->currentRequired[0]==stateCheck[0]){
        if(checkTransition->currentRequired[1]==stateCheck[1]){
            if(checkTransition->currentRequired[2]==stateCheck[2]){
                    return 1;

            }
        }
    }
    return 0;
}


//***************************************************CHECK IF STATE IN THE NODE********************************************
bool checkIfStateIn(int stateArray[],Node *check){

    for(int i=0;i<check->stateNum;i++){
        if(check->states[i][0]==stateArray[0]){
            if(check->states[i][1]==stateArray[1]){
                if(check->states[i][2]==stateArray[2]){
                    return 1;
                }
            }
        }
    }
    return 0;

}






//***************************************************GERNERATE THE OBSERVER NODE TREE***************************************
void makeNextNode(Node *start, Node *previous, pConcurrentAutomatonState concurrentAuto){

    Node *node[10];
    Node *nodeNext[10];
    int nodeFlag = 0;
    int nodeFlagNext = 0;
    int counter = 0;
    State startState[5];

    pConcurrentTransition transitionRemain[10];


    for(int i=0;i<previous->numOfEvents;i++){
        for(int j=0;j<previous->stateNum;j++){
                for(int n=0;n<concurrentAuto->transNum;n++){
                    if(concurrentAuto->_transitionTable[n]->event == previous->events[i]){
                        if(checkIfTransitionRequired(concurrentAuto->_transitionTable[n],previous->states[j])){
                                transitionRemain[nodeFlag] = (pConcurrentTransition)malloc(sizeof(concurrentTransition));

                                transitionRemain[nodeFlag]->currentRequired[0] = previous->states[j][0];
                                transitionRemain[nodeFlag]->currentRequired[1] = previous->states[j][1];
                                transitionRemain[nodeFlag]->currentRequired[2] = previous->states[j][2];

                                transitionRemain[nodeFlag]->event = previous->events[i];

                                transitionRemain[nodeFlag]->next[0] = concurrentAuto->_transitionTable[n]->next[0];
                                transitionRemain[nodeFlag]->next[1] = concurrentAuto->_transitionTable[n]->next[1];
                                transitionRemain[nodeFlag]->next[2] = concurrentAuto->_transitionTable[n]->next[2];

                                startState[0] = concurrentAuto->_transitionTable[n]->next[0];
                                startState[1] = concurrentAuto->_transitionTable[n]->next[1];
                                startState[2] = concurrentAuto->_transitionTable[n]->next[2];

                                node[nodeFlag] = makeNode(startState,concurrentAuto);
                                nodeFlag++;
                        }
                    }
                }

        }

        for(int m=0;m<nodeFlag;m++){
                if(transitionRemain[m]->event==previous->events[i]){
                    if(counter==1){
                        if(checkIfStateIn(startState,node[m])){
                            startState[0] = transitionRemain[m]->next[0];
                            startState[1] = transitionRemain[m]->next[1];
                            startState[2] = transitionRemain[m]->next[2];

                        }

                    }else if(counter == 0){
                        startState[0] = transitionRemain[m]->next[0];
                        startState[1] = transitionRemain[m]->next[1];
                        startState[2] = transitionRemain[m]->next[2];
                        counter = 1;
                    }
                }

        }
        counter = 0;
        nodeNext[nodeFlagNext] = makeNode(startState,concurrentAuto);
        nodeFlagNext++;
    }
    State startNextNode[5];
    for(int i=0;i<nodeFlagNext;i++){
            startNextNode[0] = nodeNext[i]->states[0][0];
            startNextNode[1] = nodeNext[i]->states[0][1];
            startNextNode[2] = nodeNext[i]->states[0][2];
            previous->next[i] = makeNode(startNextNode,concurrentAuto);
    }

    for(int i=0;i<nodeFlagNext;i++){
        free(nodeNext[i]);
        nodeNext[i]=NULL;
    }
    for(int i=0;i<nodeFlag;i++){
        free(node[i]);
        node[i]=NULL;
    }
}

void test(pConcurrentAutomatonState concurrentAuto){

    for(int m=0;m<concurrentAuto->transNum;m++){
        printf("\n\n %d %d %d\n   %d   \n %d %d %d\n***************\n\n",
               concurrentAuto->_transitionTable[m]->currentRequired[1],
               concurrentAuto->_transitionTable[m]->currentRequired[0],
               concurrentAuto->_transitionTable[m]->currentRequired[2],
               concurrentAuto->_transitionTable[m]->event,
               concurrentAuto->_transitionTable[m]->next[1],
               concurrentAuto->_transitionTable[m]->next[0],
               concurrentAuto->_transitionTable[m]->next[2]);

    }

    printf("\n\n*************%d******\n\n\n\n\n\n\n\n",concurrentAuto->transNum);

}



int main(int argc, char *argv[]){
    //Create the channel automaton
    int numOfTrans1 = sizeof(channel1)/sizeof(pTransition);
    pAutomatonState channelAuto1;
    channelAuto1 = makeChannel(channel1,numOfTrans1,1);

    int numOfTrans2 = sizeof(channel2)/sizeof(pTransition);
    pAutomatonState channelAuto2;
    channelAuto2 = makeChannel(channel2,numOfTrans2,2);

    pAutomatonState automaton;
    automaton = initialize(4);

    pConcurrentAutomatonState concurrentAuto1;
    concurrentAuto1 = shuffle(automaton,channelAuto1);

    //test(concurrentAuto1);
    pConcurrentAutomatonState concurrentAutomaton;
    concurrentAutomaton = concurrent(concurrentAuto1,channelAuto2);

    //test(concurrentAutomaton);
    State start[3] = {1,0,0};

    int eventOccurred;

    Node *observer = makeNode(start,concurrentAutomaton);

    makeNextNode(observer,observer,concurrentAutomaton);

    while(1){

        if(observer->stateNum==1){
            printf("\n\n\n\n\nCurrent state is %d\n",observer->state[0]);
        }else if(observer->stateNum==2){
            printf("\n\n\n\n\nCurrent state is %d %d\n",observer->state[0],observer->state[1]);
        }else if(observer->stateNum>2){
            printf("\n\n\n\n\nCurrent state is %d %d %d\n",observer->state[0],observer->state[1],observer->state[2]);
        }
        printf("In current state. These events could happen:\n");
        for(int i=0;i<observer->numOfEvents;i++){
            printf("***********\n    %d    \n",observer->events[i]);
        }
        printf("***********\n\nEnter the event occurred:");
        scanf("%d",&eventOccurred);
        for(int j=0;j<observer->numOfEvents;j++){
            if(eventOccurred==observer->events[j]){
                observer = observer->next[j];
                makeNextNode(observer,observer,concurrentAutomaton);
            }
        }
        if(eventOccurred==100){
            break;
        }
    }


    return 0;
}
