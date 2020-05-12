#pragma once

typedef void* DataType;
typedef struct linkedlistnode{
    DataType data;
    struct linkedlistnode*next;
}LinkedList;

LinkedList* createList();
void emptylist(LinkedList*);
void emptyListPro(LinkedList*,void(DataType));
void destoryList(LinkedList*);
void destoryListPro(LinkedList*,void(DataType));
int listIsEmpty(LinkedList*);
DataType searchList(LinkedList*,DataType,int(DataType,DataType));
void dropListElement(LinkedList*,DataType,int(DataType,DataType));
DataType listGet(LinkedList*,int);
int pushList(LinkedList*,DataType);
LinkedList*copyList(LinkedList*);
LinkedList*copyListPro(LinkedList*,DataType(DataType));
void printList(LinkedList*,int(DataType));