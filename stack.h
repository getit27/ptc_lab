#pragma once

typedef void* DataType;
typedef struct node{
    DataType data;
    struct node*next;
}Stack;

Stack* createStack();
void statckEmpty(Stack*);
void destoryStack(Stack*);
int stackIsEmpty(Stack*);
int pushStack(Stack*,DataType);
int popStack(Stack*);
DataType topStack(Stack*);