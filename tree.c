#include"syntax.tab.h"
#include"tree.h"
#include<stdio.h>
#include<stdlib.h>
#include<string.h>


struct TreeNode* newTreeNode(int type,int line,int subnum,struct TreeNode**subnode){
    struct TreeNode*rst=(struct TreeNode*)malloc(sizeof(struct TreeNode));
    rst->type=type;
    rst->val.type_int=0;
    rst->line=line;
    rst->subnum=subnum;
    rst->leaf=0;
    for(int i=0;i<subnum&&i<8;i++){
        rst->subnode[i]=subnode[i];
    }
    return rst;
}

struct TreeNode* newTreeIntLeaf(int type,int line,int value){
    struct TreeNode*rst=(struct TreeNode*)malloc(sizeof(struct TreeNode));
    rst->type=type;
    rst->val.type_int=value;
    rst->line=line;
    rst->subnum=0;
    rst->leaf=1;
}

struct TreeNode* newTreeFloatLeaf(int type,int line,float value){
    struct TreeNode*rst=(struct TreeNode*)malloc(sizeof(struct TreeNode));
    rst->type=type;
    rst->val.type_float=value;
    rst->line=line;
    rst->subnum=0;
    rst->leaf=1;
}

struct TreeNode* newTreeStrLeaf(int type,int line,char* value){
    struct TreeNode*rst=(struct TreeNode*)malloc(sizeof(struct TreeNode));
    rst->type=type;
    char*str=(char*)malloc(64*sizeof(char));
    strcpy(str,value);
    rst->val.type_string=str;
    rst->line=line;
    rst->subnum=0;
    rst->leaf=1;
}

void traverseTree(struct TreeNode* root,int level){
    sematicAnalysis(root);
    //ifError(root);
    return;
    char*name=getTypeName(root->type);
    if(root->leaf){
        for(int i=0;i<level;i++){
            printf("  ");
        }
        printf("%s",name);
        switch(root->type){
            case INT:   printf(": %d",root->val.type_int);break;
            case FLOAT: printf(": %f",root->val.type_float);break;
            case TYPE:  printf(": %s",root->val.type_string);break;
            case ID:    printf(": %s",root->val.type_string);break;
            case RELOP: printf(": %s",root->val.type_string);break;
        }
        printf("\n");
    }else if(root->subnum){
        for(int i=0;i<level;i++){
            printf("  ");
        }
        printf("%s ",name);
        printf("(%d)",root->line);
        printf("\n");
        for(int i=0;i<root->subnum;i++){
            traverseTree(root->subnode[i],level+1);
        }
    }
}

char*getTypeName(int code){
    char* name;
    switch(code){
        case INT:   name="INT";break;
        case FLOAT: name="FLOAT";break;
        case TYPE:  name="TYPE";break;
        case ID:    name="ID";break;
        case STRUCT:    name="STRUCT";break;
        case RETURN:    name="RETURN";break;
        case IF:    name="IF";break;
        case ELSE:  name="ELSE";break;
        case WHILE: name="WHILE";break;
        case SEMI:  name="SEMI";break;
        case COMMA: name="COMMA";break;
        case ASSIGNOP:  name="ASSIGNOP";break;
        case RELOP: name="RELOP";break;
        case PLUS:  name="PLUS";break;
        case MINUS: name="MINUS";break;
        case STAR:  name="STAR";break;
        case AND:   name="AND";break;
        case DIV:   name="DIV";break;
        case OR:    name="OR";break;
        case DOT:   name="DOT";break;
        case NOT:   name="NOT";break;
        case LP:    name="LP";break;
        case RP:    name="RP";break;
        case LB:    name="LB";break;
        case RB:    name="RB";break;
        case LC:    name="LC";break;
        case RC:    name="RC";break;
        case 0:     name="Program";break;
        case 1:
        case 2:     name="ExDefList";break;
        case 3:
        case 4:
        case 5:     name="ExtDef";break;
        case 6:
        case 7:     name="ExDecList";break;
        case 8:
        case 9:     name="Specifier";break;
        case 10:
        case 11:    name="StructSpecifier";break;
        case 12:
        case 13:    name="OptTag";break;
        case 14:    name="Tag";break;
        case 15:
        case 16:    name="VarDec";break;
        case 17:
        case 18:    name="FunDec";break;
        case 19:
        case 20:    name="VarList";break;
        case 21:    name="ParamDec";break;
        case 22:    name="CompSt";break;
        case 23:
        case 24:    name="StmtList";break;
        case 25:
        case 26:
        case 27:
        case 28:
        case 29:
        case 30:    name="Stmt";break;
        case 31:
        case 32:    name="DefList";break;
        case 33:    name="Def";break;
        case 34:
        case 35:    name="DecList";break;
        case 36:
        case 37:    name="Dec";break;
        case 38:case 39:case 40:case 41:
        case 42:case 43:case 44:case 45:
        case 46:case 47:case 48:case 49:
        case 50:case 51:case 52:case 53:
        case 54:case 55:
                    name="Exp";break;
        case 56:
        case 57:    name="Args";
        default:    name="UNKNOWN";break;
    }
    return name;
}