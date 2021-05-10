#include "State.h"

transition t1={
    STATE1,STATE2,EVENT1
};
transition t2={
    STATE2,STATE1,EVENT2
};
transition t3={
    STATE2,STATE3,EVENT3
};
transition t4={
    STATE3,STATE1,EVENT4
};

pTransition transitionTable[]={
&t1,&t2,&t3,&t4
};

pTransition channel1[3]={
&t1,&t2,&t3
};

pTransition channel2[1]={
&t4
};

void onAction(State mystate, Event event){
    printf("Event %d occurred.\n",event);
}

State step(pAutomatonState machine, Event event){
    if(event<=0||event>EVENTS){
        printf("Invalid action...\n");
        printf("\nCurrent state is %d\n",machine->current+1);
        return machine->current;
    }
    if(machine->current == machine->_transitionTable[event-1]->currentRequired){
        onAction(machine->current,event);
        machine->current = machine->_transitionTable[event-1]->next;
        printf("\nCurrent state is %d\n",machine->current+1);
    }else{
        printf("Invalid action...\n");
    }
    return machine->current;
}

/*
int main(int argc, char *argv[]){
    automatonState automaton;
    automaton.current = STATE1;
    automaton._transitionTable = (pTransition *) malloc(EVENTS * sizeof(transition));
    automaton._transitionTable = transitionTable;
    //for(int i=0;i<=EVENTS;i++){
    //    automaton._transitionTable[i] = transitionTable[i];
    //}
    int eventOccurred;
    while(1){
        printf("\n\n\n\nEnter the event:");
        scanf("%d",&eventOccurred);
        if(eventOccurred == 100) break;
        step(&automaton,eventOccurred);
    }
    free(automaton._transitionTable);
    return 0;
}
*/
