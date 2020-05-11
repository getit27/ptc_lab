#pragma once

typedef void* DataTypeTest;
typedef struct linkedlistnodeTest{
    DataTypeTest data;
    struct linkedlistnodeTest*next;
}LinkedListTest;

LinkedListTest* createListTest();
void emptylistTest(LinkedListTest*);
void emptyListProTest(LinkedListTest*,void(DataTypeTest));
void destoryListTest(LinkedListTest*);
void destoryListProTest(LinkedListTest*,void(DataTypeTest));
int listIsEmptyTest(LinkedListTest*);
DataTypeTest searchListTest(LinkedListTest*,DataTypeTest,int(DataTypeTest,DataTypeTest));
void dropListElementTest(LinkedListTest*,DataTypeTest,int(DataTypeTest,DataTypeTest));
DataTypeTest listGetTest(LinkedListTest*,int);
int pushListTest(LinkedListTest*,DataTypeTest);
LinkedListTest*copyListTest(LinkedListTest*);
LinkedListTest*copyListProTest(LinkedListTest*,DataTypeTest(DataTypeTest));
void printListTest(LinkedListTest*,int(DataTypeTest));
int printAddress(void*a);