#include"linkedlisttest.h"
#include<stdlib.h>
#include<stdio.h>

LinkedListTest* createListTest(){
    LinkedListTest*list=(LinkedListTest*)malloc(sizeof(LinkedListTest));
    if(NULL!=list){
        list->data=0;
        list->next=NULL;
        return list;
    }
    return NULL;
}

void emptylistTest(LinkedListTest* list){
    LinkedListTest*node=list->next;
    list->next=NULL;
    while(node){
        LinkedListTest*temp=node;
        node=node->next;
        free(temp);
    }
}

void emptyListProTest(LinkedListTest* list,void (destory)(DataTypeTest)){
    LinkedListTest*node=list->next;
    list->next=NULL;
    while(node){
        LinkedListTest*temp=node;
        node=node->next;
        destory(temp->data);
        free(temp);
    }
}

void destoryListTest(LinkedListTest*list){
    emptylistTest(list);
    free(list);
}

void destoryListProTest(LinkedListTest*list,void (destory)(DataTypeTest)){
    emptyListProTest(list,destory);
    free(list);
}

int listIsEmptyTest(LinkedListTest*list){
    return (list->next==NULL);
}

DataTypeTest searchListTest(LinkedListTest*list,DataTypeTest object,int(f)(DataTypeTest,DataTypeTest)){
    LinkedListTest* node=list->next;
    while(node!=NULL){
        if(f(node->data,object)){
            return node->data;
        }else{
            node=node->next;
        }
    }
    return NULL;
}

void dropListElementTest(LinkedListTest*list,DataTypeTest object,int(f)(DataTypeTest,DataTypeTest)){
    LinkedListTest* node=list->next;
    LinkedListTest* last=list;
    while(node!=NULL){
        if(f(node->data,object)){
            last->next=node->next;
            free(node);
            return;
        }else{
            last=node;
            node=node->next;
        }
    }
    printf("Not Found");
    //printListTest(list,printAddress);
}

DataTypeTest listGetTest(LinkedListTest*list,int index){
    LinkedListTest*node=list->next;
    for(int i=0;i<index;i++){
        node=node->next;
    }
    return node->data;
}

int pushListTest(LinkedListTest*list,DataTypeTest data){
    LinkedListTest* end=list;
    while(end->next)
        end=end->next;
    LinkedListTest* newst = (LinkedListTest*)malloc(sizeof(list));
    if(NULL!=newst){
        newst->next=0;
        newst->data=data;
        end->next=newst;
        return 1;
    }
    return 0;
}

LinkedListTest*copyListTest(LinkedListTest*list){
    LinkedListTest*newList=createListTest();
    LinkedListTest*dest=newList;
    LinkedListTest*src=list;
    while(src->next){
        dest->next=(LinkedListTest*)malloc(sizeof(list));
        dest->next->data=src->next->data;
        dest=dest->next;
        src=src->next;
    }
    dest->next=NULL;
    return newList;
}

LinkedListTest*copyListProTest(LinkedListTest*list,DataTypeTest(f)(DataTypeTest)){
    LinkedListTest*newList=createListTest();
    LinkedListTest*dest=newList;
    LinkedListTest*src=list;
    while(src->next){
        dest->next=(LinkedListTest*)malloc(sizeof(list));
        dest->next->data=f(src->next->data);
        dest=dest->next;
        src=src->next;
    }
    dest->next=NULL;
    return newList;
}

void printListTest(LinkedListTest*list,int(f)(DataTypeTest)){
    LinkedListTest*node=list->next;
    while(node){
        f(node->data);
        printf("\n");
        node=node->next;
    }
}