#include<stdio.h>
#include<stdlib.h>
#include"stack.h"

long myMalloc(int len);

Stack* createStack(){
    Stack*stack=(Stack*)myMalloc(sizeof(Stack));
    if(NULL!=stack){
        stack->next=NULL;
        return stack;
    }
    return NULL;
}

void stackEmpty(Stack*stack){
    while(!stackIsEmpty(stack))
    {
        popStack(stack);
    }
}

void destoryStack(Stack* stack){
    stackEmpty(stack);
    myFree(stack);
}

int stackIsEmpty(Stack* stack){
    return (stack->next==NULL);
}

int pushStack(Stack *stack,DataType data){
    Stack*newst=NULL; 
    newst = (Stack*)myMalloc(sizeof(stack));
    if(NULL!=newst){
        newst->data=data;
        newst->next=stack->next;
        stack->next=newst;
        return 1;
    }
    return 0;
}

int popStack(Stack* stack){
    Stack* tmpst;
    if(!stackIsEmpty(stack)){
        tmpst=stack->next;
        stack->next=tmpst->next;
        myFree(tmpst);
        return 1;
    }
    return 0;
}

DataType topStack(Stack* stack){
    if(!stackIsEmpty(stack)){
        return stack->next->data;
    }
    return -1;
}