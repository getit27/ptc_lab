#pragma once
#include"tree.h"
#include"stack.h"
#include"string.h"
#include"linkedlist.h"
#include"translate.h"

typedef enum{INT,FLOAT,IPOINT,FPOINT}BaseType;
typedef enum{VARIABLE,STRUCT}DataaType;

typedef struct at_{
    int type;//0123 is base type 4 is array
    struct at_*element;
    int size;
}AllType;

typedef struct var_{
    char* name;
    AllType*type;
}Variable;

typedef struct str_{
    int num;//auto
    char* name;
    LinkedList*datalist;
}Struct;

typedef struct func_{
    char*name;
    AllType*retType;
    LinkedList*paraList;//typeList
}Function;
typedef struct stackEle_{
    struct TreeNode*node;
    int level;
}StackEle;
typedef struct SpecifierNode_{
    AllType*t;
    int error;
}SpecifierNode;
typedef struct OptTagNode_{
    int opt;
    char*id;
}OptTagNode;
typedef struct TagNode_{
    char*id;
}TagNode;
typedef struct StructSpecifierNode_{
    AllType*t;
    int error;
}StructSpecifierNode;
typedef struct VarDecNode_{
    Variable*v;//type init before afferent
    int block;//true => use cnkiblock
    int error;
}VarDecNode;
typedef struct ExtDecListNode_{
    //inital
    AllType*t; //not array
    int block;
    LinkedList*varList; //to be add elem
    //sub
    int empty;//inital =1
}ExtDecListNode;
typedef struct ParamDecNode_{
    Variable*v;
    int error;
}ParamDecNode;
typedef struct ExpNode_{
    AllType*t;
    int left;
    int error;
}ExpNode;
typedef struct ArgsNode_{
    //inital
    LinkedList*typeList;
    //sub
    int error;
}ArgsNode;
typedef struct DecNode_{
    Variable*v;//type inital
    int compst;//inital
    int error;
}DecNode;
typedef struct DecListNode_{
    int compst;//inital
    AllType*type;//inital
    LinkedList*varList;//inital
    int error;
}DecListNode;
typedef struct DefNode_{
    int compst;//inital
    LinkedList*varList;//inital
    int error;
}DefNode;
typedef struct DefListNode_{
    int compst;//inital
    LinkedList*varList;//inital
    int empty;
}DefListNode;//inital
typedef struct StmtNode_{
    AllType*retType;//inital
    int error;
}StmtNode;
typedef struct StmtListNode_{
    AllType*retType;//inital
}StmtListNode;
typedef struct CompStNode_{
    LinkedList*paramList;//inital
    AllType*retType;
}CompStNode;
typedef struct VarListNode_{
    LinkedList*paramList;//inital
    int error;
}VarListNode;
typedef struct FuncDecNode_{
    Function*func;//inital type
    int empty;
}FuncDecNode;



StackEle*createStackEle(struct TreeNode*node,int level);
Struct*createStruct(char*name);
Struct*structAdd(Struct*str,Variable*var,int copy);
Struct*copyStruct(Struct*str);
Variable*copyVariable(Variable*var);
AllType*copyType(AllType*type);
Function*copyFunc(Function*func);
char*copyString(char*str);
void destoryStruct(Struct*str);
void destoryType(AllType*type);
void destoryVariable(Variable*var);
void destoryFunc(Function*func);
void sematicAnalysis(struct TreeNode*);
int addStruct2Table(Struct*str);
void addVariable2Table(Variable*var);
void addVars2Table(LinkedList*var);
void addFunction2Table(Function*func);
void addCompSt2Table();//New VAR Table
void deleteCompStFromTable();
int cnkiBlock(char*name);
int cnkiAll(char*name);
int equalByNameVar(void*var,void*name);
int equalByNameFunc(void*func,void*name);
int equalByNameStruct(void*struc,void*name);
int equalByNameStructField(void*field,void*name);
Function*getFunction(char*name);
Variable*getVariable(char*name);
Struct*getStructbName(char*name);
Struct*getStruct(int id);
int consistentType(AllType*type0,AllType*type1);
int consistenArgs(LinkedList*list0,LinkedList*list1);
void printError();
long myMalloc(int len);
void myFree(void*a);
void printStart(struct TreeNode*root);
void printEnd(struct TreeNode*root);
int compareAddress(void*a,void*b);