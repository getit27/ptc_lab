#pragma once
#include"linkedlist.h"
#include"semantic.h"
#include"stdlib.h"
#include"stdio.h"

typedef struct IdVariable{
    struct var_*variable;
    int id;
}IdVariable;

int equalByNameIdVar(void*idvar,void*name);

void destoryIdVariable(IdVariable*idvar);

typedef struct Sentence{
    char str[40];
    struct Sentence*last;
    struct Sentence*next;
}Sentence;

Sentence*codeAdd(Sentence*code1,Sentence*code2);

Sentence*translateExp(struct TreeNode*node,char*place,char*values,struct at_*types,char*address);

//Sentence*translateExpAddress(struct TreeNode*node,char*place,char*exp0,char*exp1);

Sentence*translateStmt(struct TreeNode*node);

Sentence*translateCond(struct TreeNode*node,char*ltrue,char*lfalse);

Sentence*translateArgs(struct TreeNode*node,LinkedList*arglist,LinkedList*addlist);

Sentence*translateCompst(struct TreeNode*node);

Sentence*translateStmtList(struct TreeNode*node);

Sentence*translateExtDef(struct TreeNode*node);

Sentence*translateExtDefList(struct TreeNode*node);

Sentence*translateProgram(struct TreeNode*node);

Sentence*translateDefList(struct TreeNode*node);

Sentence*translateDef(struct TreeNode*node);

Sentence*translateDecList(struct TreeNode*node);

Sentence*translateDec(struct TreeNode*node);

Sentence*translateVarDec(struct TreeNode*node,char*name);

void printCode(Sentence*code);

int shiftStruct(struct str_*stru,char*name);

int spaceStruct(struct var_*var);

int spaceArray(struct var_*var);

int spaceStructType(struct at_*type);

int spaceArrayType(struct at_*type);

int spaceType(struct at_*type);

void printInterCode(FILE*fo);

char* itoa(int num,char* str,int radix);