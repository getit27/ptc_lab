#include"linkedlist.h"
#include<stdlib.h>

long myMalloc(int len);

LinkedList* createList(){
    LinkedList*list=(LinkedList*)myMalloc(sizeof(LinkedList));
    if(NULL!=list){
        list->data=0;
        list->next=NULL;
        return list;
    }
    return NULL;
}

void emptylist(LinkedList* list){
    LinkedList*node=list->next;
    list->next=NULL;
    while(node){
        LinkedList*temp=node;
        node=node->next;
        myFree(temp);
    }
}

void emptyListPro(LinkedList* list,void (destory)(DataType)){
    LinkedList*node=list->next;
    list->next=NULL;
    while(node){
        LinkedList*temp=node;
        node=node->next;
        destory(temp->data);
        myFree(temp);
    }
}

void destoryList(LinkedList*list){
    emptylist(list);
    myFree(list);
}

void destoryListPro(LinkedList*list,void (destory)(DataType)){
    emptyListPro(list,destory);
    myFree(list);
}

int listIsEmpty(LinkedList*list){
    return (list->next==NULL);
}

DataType searchList(LinkedList*list,DataType object,int(f)(DataType,DataType)){
    LinkedList* node=list->next;
    while(node!=NULL){
        if(f(node->data,object)){
            return node->data;
        }else{
            node=node->next;
        }
    }
    return NULL;
}

void dropListElement(LinkedList*list,DataType object,int(f)(DataType,DataType)){
    LinkedList* node=list->next;
    LinkedList* last=list;
    while(node!=NULL){
        if(f(node->data,object)){
            last->next=node->next;
            myFree(node);
            return;
        }else{
            last=node;
            node=node->next;
        }
    }
    printf("Not Found");
}

DataType listGet(LinkedList*list,int index){
    LinkedList*node=list->next;
    for(int i=0;i<index;i++){
        node=node->next;
    }
    return node->data;
}

int pushList(LinkedList*list,DataType data){
    LinkedList* end=list;
    while(end->next)
        end=end->next;
    LinkedList* newst = (LinkedList*)myMalloc(sizeof(list));
    if(NULL!=newst){
        newst->next=0;
        newst->data=data;
        end->next=newst;
        return 1;
    }
    return 0;
}

LinkedList*copyList(LinkedList*list){
    LinkedList*newList=createList();
    LinkedList*dest=newList;
    LinkedList*src=list;
    while(src->next){
        dest->next=(LinkedList*)myMalloc(sizeof(list));
        dest->next->data=src->next->data;
        dest=dest->next;
        src=src->next;
    }
    dest->next=NULL;
    return newList;
}

LinkedList*copyListPro(LinkedList*list,DataType(f)(DataType)){
    LinkedList*newList=createList();
    LinkedList*dest=newList;
    LinkedList*src=list;
    while(src->next){
        dest->next=(LinkedList*)myMalloc(sizeof(list));
        dest->next->data=f(src->next->data);
        dest=dest->next;
        src=src->next;
    }
    dest->next=NULL;
    return newList;
}

void printList(LinkedList*list,int(f)(DataType)){
    LinkedList*node=list->next;
    while(node){
        f(node->data);
        printf("\n");
        node=node->next;
    }
}