#include"tree.h"
#include"semantic.h"
#include"linkedlisttest.h"
#include"stack.h"
#include<stdio.h>
#define NEW(a) (a*)myMalloc(sizeof(a))

Sentence*interCode;
int varid=0;
LinkedList*structTable;
int strNum;
LinkedList*functionTable;
Stack*variableTables;
int nmalloc=0;
int nfree=0;
LinkedListTest*mallocTable;
LinkedList*translateTable;

void sematicAnalysis(struct TreeNode*root){
    printStart(root);
    //Tables
    //myMalloc(762);
    mallocTable=createListTest();
    structTable=createList();
    functionTable=createList();

    Function*funcWrite=NEW(Function);
    funcWrite->name=myMalloc(6*sizeof(char));
    strcpy(funcWrite->name,"write");
    funcWrite->paraList=createList();
    Variable*writeI=NEW(Variable);
    writeI->name=myMalloc(2*sizeof(char));
    strcpy(writeI->name,"i");
    writeI->type=NEW(AllType);
    writeI->type->type=INT;
    pushList(funcWrite->paraList,writeI);
    funcWrite->retType=NEW(AllType);
    funcWrite->retType->type=INT;
    Function*funcRead=NEW(Function);
    funcRead->name=myMalloc(5*sizeof(char));
    strcpy(funcRead->name,"read");
    funcRead->paraList=createList();
    funcRead->retType=NEW(AllType);
    funcRead->retType->type=INT;
    pushList(functionTable,funcWrite);
    pushList(functionTable,funcRead);

    variableTables=createStack();
    translateTable=createList();
    LinkedList*newVariable=createList();
    strNum=5;
    pushStack(variableTables,newVariable);
    //Analyse
    if(root->type==0){
        sematicGeneExtDefList(root->subnode[0],0);
        interCode=translateProgram(root);
        printCode(interCode);
    }else{
        printf("Error: invalid type");
        exit(1);
    }
    //printf("free struct table:\n");
    destoryListPro(structTable,destoryStruct);
    //printf("free func table:\n");
    destoryListPro(functionTable,destoryFunc);
    //printf("free variable table\n");
    destoryListPro(variableTables->next->data,destoryVariable);
    destoryStack(variableTables);
    //destoryListPro(translateTable,destoryIdVariable);
    //printf("unfree:\n");
    printListTest(mallocTable,printAddress);
    printEnd(root);
}

void sematicGeneExtDefList(struct TreeNode*root,void*attribute){
    printStart(root);
    if(root->type==1){//ExtDef ExtDefList
        sematicGeneExtDef(root->subnode[0],0);
        sematicGeneExtDefList(root->subnode[1],0);
    }else if(root->type==2){//
        
    }else{
        printf("Error: invalid type");
        exit(1);
    }
    printEnd(root);
}//ExtDefList

void sematicGeneExtDef(struct TreeNode*root,void*attribute){
    printStart(root);
    if(root->type==3){//Specifier ExtDecList SEMI
        SpecifierNode*subnode0=NEW(SpecifierNode);
        sematicGeneSpecifier(root->subnode[0],subnode0);
        if(!subnode0->error){
            ExtDecListNode*subnode1=NEW(ExtDecListNode);
            subnode1->block=0;
            subnode1->empty=1;
            subnode1->t=subnode0->t;
            subnode1->varList=createList();
            sematicGeneExtDecList(root->subnode[1],subnode1);
            if(!subnode1->empty){
                addVars2Table(subnode1->varList);
            }
            destoryListPro(subnode1->varList,destoryVariable);
            myFree(subnode1);
            destoryType(subnode0->t);
        }
        myFree(subnode0);
    }else if(root->type==4){//Specifier SEMI
        SpecifierNode*subnode=NEW(SpecifierNode);
        sematicGeneSpecifier(root->subnode[0],subnode);   
        if(!subnode->error){
            destoryType(subnode->t);
        }
        myFree(subnode);
    }else if(root->type==5){//Specifier FunDec CompSt
        SpecifierNode*subnode0=NEW(SpecifierNode); 
        sematicGeneSpecifier(root->subnode[0],subnode0);
        if(subnode0->error){
            subnode0->t=NEW(AllType);
            subnode0->t->type=3;
        }
        FuncDecNode*subnode1=NEW(FuncDecNode);
        subnode1->func=NEW(Function);
        subnode1->func->retType=subnode0->t;
        sematicGeneFunDec(root->subnode[1],subnode1);

        if(!getFunction(subnode1->func->name)){
            addFunction2Table(subnode1->func);
        }else{
            printf("Error type 4 at line %d: Redefined function \"%s\"\n",root->line,subnode1->func->name);
            printError();
            //ERRORB
        }
            
        CompStNode*subnode2=NEW(SpecifierNode);
        subnode2->paramList=subnode1->func->paraList;
        subnode2->retType=subnode1->func->retType;
        sematicGeneCompSt(root->subnode[2],subnode2);

        destoryFunc(subnode1->func);
        myFree(subnode2);
        myFree(subnode1);
        
        myFree(subnode0);
    }else{
        printf("Error: invalid type");
        exit(1);
    }
    printEnd(root);
    
}//ExDef

void sematicGeneExtDecList(struct TreeNode*root,void*attribute){
    printStart(root);
    ExtDecListNode*node=(ExtDecListNode*)attribute;
    //node->error=1;
    if(root->type==6){//VarDec
        VarDecNode*subnode0=NEW(VarDecNode);
        subnode0->v=NEW(Variable);
        subnode0->v->type=copyType(node->t);
        subnode0->block=node->block;
        sematicGeneVarDec(root->subnode[0],subnode0);
        if(!subnode0->error){
            pushList(node->varList,subnode0->v);
            node->empty=0;
        }
        myFree(subnode0);
    }else if(root->type==7){//VarDec COMMA ExtDecList
        VarDecNode*subnode0=NEW(VarDecNode);
        subnode0->v->type=copyType(node->t);
        subnode0->block=node->block;
        sematicGeneVarDec(root->subnode[0],subnode0);
        if(!subnode0->error){
            pushList(node->varList,subnode0->v);
            node->empty=0;
        }
        myFree(subnode0);
        sematicGeneExtDecList(root->subnode[2],node);
    }else{
        printf("Error: invalid type");
        exit(1);
    }
    printEnd(root);
}

void sematicGeneSpecifier(struct TreeNode*root,void*attribute){
    printStart(root);
    SpecifierNode*node=(SpecifierNode*)attribute;
    if(root->type==8){//TYPE
        node->error=0;
        ((SpecifierNode*)attribute)->t=NEW(AllType);
        if((root->subnode[0]->val.type_string)[0]=='i')
            ((SpecifierNode*)attribute)->t->type=INT;
        else if((root->subnode[0]->val.type_string)[0]=='f')
            ((SpecifierNode*)attribute)->t->type=FLOAT;
        else{
            printf("Error: Unknown basic data type");
            exit(1);
        }
    }else if(root->type==9){//StructSpecifier
        node->t=0;
        StructSpecifierNode*subnode=NEW(StructSpecifierNode);
        subnode->t=NEW(AllType);
        sematicGeneStructSpecifier(root->subnode[0],subnode);
        if(subnode->error){
            node->error=1;
            //ERRORA
        }else{
            node->error=0;
            node->t=copyType(subnode->t);
        }
        myFree(subnode->t);
        myFree(subnode);     

        //Struct Specifier
    }else{
        printf("Error: invalid type");
        exit(1);
    }
    printEnd(root);
}

void sematicGeneStructSpecifier(struct TreeNode*root,void*attribute){
    printStart(root);
    StructSpecifierNode*node=(StructSpecifierNode*)attribute;
    if(root->type==10){//STRUCT OptTag LC DefList RC
        OptTagNode*subnode0=NEW(OptTagNode);
        sematicGeneOptTag(root->subnode[1],subnode0);
        char*strName;
        if(subnode0->opt){
            strName=copyString(subnode0->id);
            myFree(subnode0->id);
        }else{
            strName=NEW(char);
            *strName=0;//set name empty
        }
        if(subnode0->opt&&!cnkiAll(strName)){
            //ERRORB
            printf("Error type 16 at line %d: Duplicated name \"%s\"\n",root->line,strName);
            printError();
            node->error=1;
        }else{
            DefListNode*subnode1=NEW(DefListNode);
            subnode1->varList=createList();
            sematicGeneDefList(root->subnode[3],subnode1);

            int type;
            node->error=0;
            Struct*newstruct=NEW(Struct);
            newstruct->name=copyString(strName);
            newstruct->datalist=copyListPro(subnode1->varList,copyVariable);

            type=addStruct2Table(newstruct);
            destoryType(node->t);
            node->t=NEW(AllType);
            node->t->type=type;
            destoryStruct(newstruct);
            destoryListPro(subnode1->varList,destoryVariable);
            myFree(subnode1);

        }
        myFree(strName);
        myFree(subnode0);

    }else if(root->type==11){//STRUCT Tag
        TagNode*subnode=NEW(TagNode);
        sematicGeneTag(root->subnode[1],subnode);
        Struct*str=getStructbName(subnode->id);
        if(str){
            destoryType(node->t);
            node->t->type=str->num;
            node->error=0;
        }else{
            //ERRORB STRUCT NOT FOUND
            printf("Error type 17 at line %d: Undefined struct \"%s\"\n",root->line,subnode->id);
            node->error=1;
        }
    }else{
        printf("Error: invalid type");
        exit(1);
    }
    printEnd(root);
}

void sematicGeneOptTag(struct TreeNode*root,void*attribute){
    printStart(root);
    if(root->type==12){//ID
        struct TreeNode*leaf=root->subnode[0];
        char*str=leaf->val.type_string;
        ((OptTagNode*)attribute)->opt=1;
        ((OptTagNode*)attribute)->id=(char*)myMalloc(sizeof(char)*(strlen(str)+1));
        strcpy(((OptTagNode*)attribute)->id,str);
    }else if(root->type==13){//
        ((OptTagNode*)attribute)->opt=0;
    }else{;
        exit(1);
    }
    printEnd(root);
}

void sematicGeneTag(struct TreeNode*root,void*attribute){
    printStart(root);
    TagNode*node=(TagNode*)attribute;
    if(root->type==14){//ID
        struct TreeNode*leaf=root->subnode[0];
        char*name=copyString(leaf->val.type_string);
        node->id=name;
    }else{
        printf("Error: invalid type");
        exit(1);
    }
    printEnd(root);
}

void sematicGeneVarDec(struct TreeNode*root,void*attribute){
    printStart(root);
    VarDecNode*node=(VarDecNode*)attribute;
    if(root->type==15){//ID
        struct TreeNode*leaf=root->subnode[0];
        char*nametemp=leaf->val.type_string;
        if(node->block&&cnkiBlock(nametemp)||!node->block&&cnkiAll(nametemp)){
            node->v->name=(char*)myMalloc(sizeof(char)*(strlen(nametemp)+1));
            strcpy(node->v->name,nametemp);
            node->error=0;
        }else if(node->block){
            //ERRORB        
            printf("Error type 3 at line %d: Redefined variable \"%s\"\n",root->line,nametemp);
            printError();

            node->error=1;
        }else{
            //ERRORB        
            printf("Error type 15 at line %d: Redefined field: %s\n",root->line,nametemp);
            printError();

            node->error=1;
        }
    }else if(root->type==16){//VarDec LB INT RB
        VarDecNode*subnode=(VarDecNode*)myMalloc(sizeof(VarDecNode));
        AllType*typeTemp=node->v->type;
        subnode->v=NEW(Variable);
        subnode->v->type=copyType(typeTemp);
        subnode->block=node->block;
        sematicGeneVarDec(root->subnode[0],subnode);
        if(subnode->error){
            //ERRORA
            node->error=1;
        }else
        {
            node->v->name=copyString(subnode->v->name);
            int n=root->subnode[2]->val.type_int;
            AllType*newType=NEW(AllType);
            newType->size=n;
            newType->element=copyType(subnode->v->type);
            newType->type=4;
            node->v->type=newType;
            node->error=0;
            myFree(subnode);
        }
    }else{
        printf("Error: invalid type");
        exit(1);
    }
    printEnd(root);
}//func and struct

void sematicGeneFunDec(struct TreeNode*root,void*attribute){
    printStart(root);
    FuncDecNode*node=(FuncDecNode*)attribute;
    if(root->type==17){//ID LP VarList RP
        struct TreeNode*leaf=root->subnode[0];
        node->func->name=copyString(leaf->val.type_string);
        VarListNode*subnode=NEW(VarListNode);
        subnode->paramList=createList();
        sematicGeneVarList(root->subnode[2],subnode);
        node->empty=subnode->error;
        node->func->paraList=subnode->paramList;
        myFree(subnode);
    }else if(root->type==18){//ID LP RP
        struct TreeNode*leaf=root->subnode[0];
        node->func->name=copyString(leaf->val.type_string);
        node->empty=1;
        node->func->paraList=createList();

    }else{
        printf("Error: invalid type");
        exit(1);
    }
    printEnd(root);
}

void sematicGeneVarList(struct TreeNode*root,void*attribute){
    printStart(root);
    VarListNode*node=(VarListNode*)attribute;
    node->error=1;
    if(root->type==19){//ParamDec COMMA VarList
        ParamDecNode*subnode0=NEW(ParamDecNode);
        sematicGeneParamDec(root->subnode[0],subnode0);
        if(!subnode0->error){
            pushList(node->paramList,subnode0->v);
            node->error=0;
        }
        myFree(subnode0);
        sematicGeneVarList(root->subnode[2],node);
    }else if(root->type==20){//PraramDec
        ParamDecNode*subnode0=NEW(ParamDecNode);
        sematicGeneParamDec(root->subnode[0],subnode0);
        if(!subnode0->error){
            pushList(node->paramList,subnode0->v);
            node->error=0;
        }
        myFree(subnode0);
    }else{
        printf("Error: invalid type");
        exit(1);
    }
    printEnd(root);
}

void sematicGeneParamDec(struct TreeNode*root,void*attribute){
    printStart(root);
    ParamDecNode*node=(ParamDecNode*)attribute;

    if(root->type==21){//Specifier VarDec
        SpecifierNode*subnode0=NEW(SpecifierNode);
        sematicGeneSpecifier(root->subnode[0],subnode0);
        VarDecNode*subnode1=NEW(VarDecNode);
        subnode1->block=1;
        subnode1->v=NEW(Variable);
        subnode1->v->type=copyType(subnode0->t);
        sematicGeneVarDec(root->subnode[1],subnode1);
        if(subnode1->error){
            destoryType(subnode0->t);
            myFree(subnode0);
            if(subnode1->v)
                destoryVariable(subnode1->v);
            myFree(subnode1);
            node->error=1;
            return;
        }
        node->v=copyVariable(subnode1->v);
        node->error=0;
        destoryType(subnode0->t);
        myFree(subnode0);
        if(subnode1->v)
            destoryVariable(subnode1->v);
        myFree(subnode1);

    }else{
        printf("Error: invalid type");
        exit(1);
    }
    printEnd(root);
}

void sematicGeneCompSt(struct TreeNode*root,void*attribute){
    printStart(root);
    CompStNode*node=(CompStNode*)attribute;
    if(root->type==22){//LC DefList StmtList RC
        addCompSt2Table();
        addVars2Table(node->paramList);
        DefListNode*subnode0=NEW(DefListNode);
        subnode0->empty=1;
        subnode0->varList=createList();
        subnode0->compst=1;
        sematicGeneDefList(root->subnode[1],subnode0);
        //addVars2Table(subnode0->varList);
        destoryListPro(subnode0->varList,destoryVariable);
        myFree(subnode0);

        StmtListNode*subnode1=NEW(StmtListNode);
        subnode1->retType=copyType(node->retType);
        sematicGeneStmtList(root->subnode[2],subnode1);
        deleteCompStFromTable();
        destoryType(subnode1->retType);
        myFree(subnode1);
    }else{
        printf("Error: invalid type");
        exit(1);
    }
    printEnd(root);
}

void sematicGeneStmtList(struct TreeNode*root,void*attribute){
    printStart(root);
    StmtListNode*node=(StmtListNode*)attribute;
    if(root->type==23){//Stmt StmtList
        StmtNode*subnode0=NEW(StmtNode);
        subnode0->retType=copyType(node->retType);
        sematicGeneStmt(root->subnode[0],subnode0);
        destoryType(subnode0->retType);
        sematicGeneStmtList(root->subnode[1],node);
        myFree(subnode0);
    }else if(root->type==24){//
    
    }
    else{
        printf("Error: invalid type");
        exit(1);
    }
    printEnd(root);
}

void sematicGeneStmt(struct TreeNode*root,void*attribute){
    printStart(root);
    StmtNode*node=(StmtNode*)attribute;
    node->error=0;
    if(root->type==25){//Exp SEMI
        ExpNode*subnode=NEW(ExpNode);
        sematicGeneExp(root->subnode[0],subnode);
        myFree(subnode);
    }else if(root->type==26){//CompSt
        CompStNode*subnode=NEW(CompStNode);
        subnode->retType=copyType(node->retType);
        subnode->paramList=0;
        sematicGeneCompSt(root->subnode[0],subnode);
        destoryType(subnode->retType);
        myFree(subnode);
    }else if(root->type==27){//RETURN Exp SEMI
        ExpNode*subnode=NEW(ExpNode);
        sematicGeneExp(root->subnode[1],subnode);
        if(!subnode->error){
            if(!consistentType(node->retType,subnode->t)){
                node->error=1;
                printf("Error type 8 at line %d: The return type mismatched\n",root->line);
                printError();
                //ERRORB
            }
            myFree(subnode->t);
        }else{
            node->error=1;
            //ERRORA
        }
        myFree(subnode);
    }else if(root->type==28){//IF LP Exp RP Stmt
        ExpNode*subnode0=NEW(ExpNode);
        sematicGeneExp(root->subnode[2],subnode0);
        if(!subnode0->error){
            if(subnode0->t->type!=INT)
                node->error=1;
                //ERRORB
            myFree(subnode0->t);
        }else{
            node->error=1;
            //ERRORA
        }
        StmtNode*subnode=NEW(StmtNode);
        subnode->retType=copyType(node->retType);
        sematicGeneStmt(root->subnode[4],subnode);
        destoryType(subnode->retType);
        myFree(subnode);
        myFree(subnode0);
    }else if(root->type==29){//IF LP Exp RP Stmt ELES Stmt
        ExpNode*subnode0=NEW(ExpNode);
        sematicGeneExp(root->subnode[2],subnode0);
        if(!subnode0->error){
            if(subnode0->t->type!=INT)
            node->error=1;
            //ERRORB
        }else{
            node->error=1;
            //ERRORA
        }
        StmtNode*subnode1=NEW(StmtNode);
        subnode1->retType=copyType(node->retType);
        sematicGeneStmt(root->subnode[4],subnode1);
        destoryType(subnode1->retType);
        myFree(subnode1);
        StmtNode*subnode2=NEW(StmtNode);
        subnode2->retType=copyType(node->retType);
        sematicGeneStmt(root->subnode[6],subnode2);
        destoryType(subnode2->retType);
        myFree(subnode2);
    }else if(root->type==30){//WHILE LP Exp RP Stmt
        ExpNode*subnode0=NEW(ExpNode);
        sematicGeneExp(root->subnode[2],subnode0);
        if(!subnode0->error){
            if(subnode0->t->type!=INT)
            node->error=1;
            //ERRORB
        }else{
            node->error=1;
            //ERRORA
        }
        StmtNode*subnode1=NEW(StmtNode);
        subnode1->retType=copyType(node->retType);
        sematicGeneStmt(root->subnode[4],subnode1);
        destoryType(subnode1->retType);
        myFree(subnode1);
    }else{
        printf("Error: invalid type");
        exit(1);
    }
    printEnd(root);
}

void sematicGeneDefList(struct TreeNode*root,void*attribute){
    printStart(root);
    DefListNode*node=(DefListNode*)attribute;
    if(root->type==31){//Def DefList
        DefNode*subnode=NEW(DefNode);
        subnode->compst=node->compst;
        sematicGeneDef(root->subnode[0],subnode);
        if(!subnode->error){
            LinkedList*lnode=subnode->varList->next;
            while(lnode!=NULL){
                if(!searchList(node->varList,((Variable*)(lnode->data))->name,equalByNameVar)){
                    pushList(node->varList,copyVariable(lnode->data));
                    addVariable2Table(lnode->data);
                    node->empty=0;
                }else if(node->compst){
                    //ERRORB        
                    printf("Error type 3 at line %d: Redefined variable \"%s\"\n",root->line,((Variable*)(lnode->data))->name);
                    printError();
                }else{
                    //ERRORB        
                    printf("Error type 15 at line %d: Redefined field: %s\n",root->line,((Variable*)(lnode->data))->name);
                    printError();
                }
                lnode=lnode->next;
            }
        }
        if(!subnode->error){
            destoryListPro(subnode->varList,destoryVariable);
        }
        myFree(subnode);
        sematicGeneDefList(root->subnode[1],node);
    }else if(root->type==32){//

    }else{
        printf("Error: invalid type");
        exit(1);
    }
    printEnd(root);
}

void sematicGeneDef(struct TreeNode*root,void*attribute){
    printStart(root);
    DefNode*node=(DefNode*)attribute;   
    if(root->type==33){//Specifier DecList SEMI
        SpecifierNode*subnode0=NEW(SpecifierNode);
        sematicGeneSpecifier(root->subnode[0],subnode0);
        if(!subnode0->error){
            DecListNode*subnode1=NEW(DecListNode);
            subnode1->type=copyType(subnode0->t);
            subnode1->compst=node->compst;
            subnode1->varList=createList();
            sematicGeneDecList(root->subnode[1],subnode1);
            if(!subnode1->error){
                node->error=0;
                node->varList=copyListPro(subnode1->varList,copyVariable);
            }else{
                node->error=1;
                //ERRORA
            }
            destoryType(subnode1->type);
            destoryListPro(subnode1->varList,destoryVariable);
            myFree(subnode1);
        }else{
            node->error=1;
            //ERRORA
        }
        destoryType(subnode0->t);
        myFree(subnode0);
    }else{
        printf("Error: invalid type");
        exit(1);
    } 
    printEnd(root);
}

void sematicGeneDecList(struct TreeNode*root,void*attribute){
    printStart(root);
    DecListNode*node=(DecListNode* )attribute;
    node->error=1;
    if(root->type==34){//Dec
        DecNode*subnode=NEW(DecNode);
        subnode->v=NEW(Variable);
        subnode->v->name=NEW(char);
        subnode->compst=node->compst;
        subnode->v->type=copyType(node->type);
        sematicGeneDec(root->subnode[0],subnode);
        if(!subnode->error){
            pushList(node->varList,copyVariable(subnode->v));
            node->error=0;
        }
        if(subnode->v)
            destoryVariable(subnode->v);
        myFree(subnode);
    }else if(root->type==35){//Dec COMMA DecList
        DecNode*subnode0=NEW(DecNode);
        subnode0->compst=node->compst;
        subnode0->v=NEW(Variable);
        subnode0->v->name=NEW(char);
        subnode0->v->type=copyType(node->type);
        sematicGeneDec(root->subnode[0],subnode0);
        if(!subnode0->error){
            pushList(node->varList,copyVariable(subnode0->v));
            node->error=0;
        }
        if(subnode0->v)
            destoryVariable(subnode0->v);
        myFree(subnode0);

        sematicGeneDecList(root->subnode[2],attribute);
    }else{
        printf("Error: invalid type");
        exit(1);
    }
    printEnd(root);
}

void sematicGeneDec(struct TreeNode*root,void*attribute){
    printStart(root);
    DecNode*node=(DecNode*)attribute;
    if(root->type==36){//VarDec
        VarDecNode*subnode=NEW(VarDecNode);
        subnode->v=NEW(Variable);
        subnode->v->type=copyType(node->v->type);
        subnode->block=(node->compst);
        sematicGeneVarDec(root->subnode[0],subnode);
        if(!subnode->error){
            destoryVariable(node->v);
            node->v=copyVariable(subnode->v);
            destoryVariable(subnode->v);
            myFree(subnode);
            node->error=0;
        }else{
            node->error=1;
        }
    }else if(root->type==37){//VarDec ASSIGNOP Exp
        if(node->compst){
            VarDecNode*subnode0=NEW(VarDecNode);
            subnode0->v=NEW(Variable);
            subnode0->v->type=copyType(node->v->type);
            subnode0->block=(node->compst);
            sematicGeneVarDec(root->subnode[0],subnode0);
            if(!subnode0->error){
                ExpNode*subnode1=NEW(ExpNode);
                sematicGeneExp(root->subnode[2],subnode1);
                if(subnode1->error){
                    node->error=1;
                    //ERRORA
                }else if(subnode0->v->type->type!=subnode1->t->type||
                subnode0->v->type->type>2){
                    node->error=1;
                    //ERRORB
                }else{
                    destoryType(node->v->type);
                    node->v=copyVariable(subnode0->v);
                    node->error=0;
                }
                destoryType(subnode1->t);
                myFree(subnode1);
            }else{
                node->error=1;
                //ERRORA
            }
            destoryVariable(subnode0->v);
            myFree(subnode0);
        }else{
            //ERRORB
            node->error=1;
            printf("Error type 15 at line %d: Initialized struct field\n",root->line);
        }
    }else{
        printf("Error: invalid type");
        exit(1);
    }
    printEnd(root);
}

void sematicGeneExp(struct TreeNode*root,void*attribute){
    printStart(root);
    ExpNode*node=(ExpNode*)attribute;
    ExpNode*subnode0;
    ExpNode*subnode1;
    char*funcname;
    struct TreeNode*leaf;
    Function*func;
    Variable*var;
    Struct*str;
    
    ArgsNode*subnode3;
    switch(root->type){
    case 38://Exp ASSIGNOP Exp
        node->left=0;
        subnode0=NEW(ExpNode);
        subnode1=NEW(ExpNode);
        subnode0->t=subnode1->t=0;
        sematicGeneExp(root->subnode[0],subnode0);
        sematicGeneExp(root->subnode[2],subnode1);
        if(!subnode0->left){
            node->error=1;
            printf("Error type 6 at line %d: The left-hand side of an assignment must be a variable\n",root->line);
        }else if(subnode0->error||subnode1->error){
            node->error=1;
            //ERRORA
        }else if(subnode0->t->type!=subnode1->t->type){
            node->error=1;
            printf("Error type 5 at line %d: Type mismatched\n",root->line);
            printError();
            //ERRORB
        }else if(subnode0->t->type>2){
            node->error=1;
            //ERRORB
        }else{
            node->t=copyType(subnode0->t);
            node->error=0;
        }
        if(!subnode0->error)
            destoryType(subnode0->t);
        if(!subnode1->error)
            destoryType(subnode1->t);
        myFree(subnode0);
        myFree(subnode1);
        break;
    case 39:
        node->left=0;
        subnode0=NEW(ExpNode);
        subnode1=NEW(ExpNode);
        sematicGeneExp(root->subnode[0],subnode0);
        sematicGeneExp(root->subnode[2],subnode1);
        if(subnode0->error||subnode1->error){
            node->error=1;
            //ERRORA
        }else if(subnode0->t->type!=subnode1->t->type){
            node->error=1;
            printf("Error type 7 at line %d: Oprands type mismatched\n",root->line);
            printError();
            //ERRORB
        }else if(subnode0->t->type>0){
            node->error=1;
            printf("Error type 7 at line %d: Oprands type mismatched\n",root->line);
            printError();
            //ERRORB
        }else{
            node->t=copyType(subnode0->t);
            node->error=0;
        }
        if(!subnode0->error)
            destoryType(subnode0->t);
        if(!subnode1->error)
            destoryType(subnode1->t);
        myFree(subnode0);
        myFree(subnode1);
        break;
    case 40:
        node->left=0;
        subnode0=NEW(ExpNode);
        subnode1=NEW(ExpNode);
        sematicGeneExp(root->subnode[0],subnode0);
        sematicGeneExp(root->subnode[2],subnode1);
        if(subnode0->error||subnode1->error){
            node->error=1;
            //ERRORA
        }else if(subnode0->t->type!=subnode1->t->type){
            node->error=1;
            printf("Error type 7 at line %d: Oprands type mismatched\n",root->line);
            printError();
            //ERRORB
        }else if(subnode0->t->type>0){
            node->error=1;
            printf("Error type 7 at line %d: Oprands type mismatched\n",root->line);
            printError();
            //ERRORB
        }else{
            node->t=copyType(subnode0->t);
            node->error=0;
        }
        if(!subnode0->error)
            destoryType(subnode0->t);
        if(!subnode1->error)
            destoryType(subnode1->t);
        myFree(subnode0);
        myFree(subnode1);
        break;
    case 41://Exp RELOP Exp
        node->left=0;
        subnode0=NEW(ExpNode);
        subnode1=NEW(ExpNode);
        sematicGeneExp(root->subnode[0],subnode0);
        sematicGeneExp(root->subnode[2],subnode1);
        if(subnode0->error||subnode1->error){
            node->error=1;
            //ERRORA
        }else if(subnode0->t->type!=subnode1->t->type){
            node->error=1;
            printf("Error type 7 at line %d: Oprands type mismatched\n",root->line);
            printError();
            //ERRORB
        }else if(subnode0->t->type>0){
            node->error=1;
            printf("Error type 7 at line %d: Oprands type mismatched\n",root->line);
            printError();
            //ERRORB
        }else{
            node->t=copyType(subnode0->t);
            node->error=0;
        }
        if(!subnode0->error)
            destoryType(subnode0->t);
        if(!subnode1->error)
            destoryType(subnode1->t);
        myFree(subnode0);
        myFree(subnode1);
        break;
    case 42://Exp PLUS Exp
        node->left=0;
        subnode0=NEW(ExpNode);
        subnode1=NEW(ExpNode);
        sematicGeneExp(root->subnode[0],subnode0);
        sematicGeneExp(root->subnode[2],subnode1);
        if(subnode0->error||subnode1->error){
            node->error=1;
            //ERRORA
        }else if(subnode0->t->type!=subnode1->t->type){
            node->error=1;
            printf("Error type 7 at line %d: Oprands type mismatched\n",root->line);
            printError();
            //ERRORB
        }else if(subnode0->t->type>2){
            node->error=1;
            printf("Error type 7 at line %d: Oprands type mismatched\n",root->line);
            printError();
            //ERRORB
        }else{
            node->t=copyType(subnode0->t);
            node->error=0;
        }
        if(!subnode0->error)
            destoryType(subnode0->t);
        if(!subnode1->error)
            destoryType(subnode1->t);
        myFree(subnode0);
        myFree(subnode1);
        break;
    case 43:
        node->left=0;
        subnode0=NEW(ExpNode);
        subnode1=NEW(ExpNode);
        sematicGeneExp(root->subnode[0],subnode0);
        sematicGeneExp(root->subnode[2],subnode1);
        if(subnode0->error||subnode1->error){
            node->error=1;
            //ERRORA
        }else if(subnode0->t->type!=subnode1->t->type){
            node->error=1;
            printf("Error type 7 at line %d: Oprands type mismatched\n",root->line);
            printError();
            //ERRORB
        }else if(subnode0->t->type>2){
            node->error=1;
            printf("Error type 7 at line %d: Oprands type mismatched\n",root->line);
            printError();
            //ERRORB
        }else{
            node->t=copyType(subnode0->t);
            node->error=0;
        }
        if(!subnode0->error)
            destoryType(subnode0->t);
        if(!subnode1->error)
            destoryType(subnode1->t);
        myFree(subnode0);
        myFree(subnode1);
        break;
    case 44://Exp STAR Exp
        node->left=0;
        subnode0=NEW(ExpNode);
        subnode1=NEW(ExpNode);
        sematicGeneExp(root->subnode[0],subnode0);
        sematicGeneExp(root->subnode[2],subnode1);
        if(subnode0->error||subnode1->error){
            node->error=1;
            //ERRORA
        }else if(subnode0->t->type!=subnode1->t->type){
            node->error=1;
            printf("Error type 7 at line %d: Oprands type mismatched\n",root->line);
            printError();
            //ERRORB
        }else if(subnode0->t->type>2){
            node->error=1;
            printf("Error type 7 at line %d: Oprands type mismatched\n",root->line);
            printError();
            //ERRORB
        }else{
            node->t=copyType(subnode0->t);
            node->error=0;
        }
        if(!subnode0->error)
            destoryType(subnode0->t);
        if(!subnode1->error)
            destoryType(subnode1->t);
        myFree(subnode0);
        myFree(subnode1);
        break;
    case 45://Exp DIV Exp
        node->left=0;
        subnode0=NEW(ExpNode);
        subnode1=NEW(ExpNode);
        sematicGeneExp(root->subnode[0],subnode0);
        sematicGeneExp(root->subnode[2],subnode1);
        if(subnode0->error||subnode1->error){
            node->error=1;
            //ERRORB
        }else if(subnode0->t->type!=subnode1->t->type){
            node->error=1;
            printf("Error type 7 at line %d: Oprands type mismatched\n",root->line);
            printError();
            //ERRORB
        }else if(subnode0->t->type>2){
            node->error=1;
            printf("Error type 7 at line %d: Oprands type mismatched\n",root->line);
            printError();
            //ERRORB
        }else{
            node->t=copyType(subnode0->t);
            node->error=0;
        }
        if(!subnode0->error)
            destoryType(subnode0->t);
        if(!subnode1->error)
            destoryType(subnode1->t);
        myFree(subnode0);
        myFree(subnode1);
        break;
    case 46:
        subnode0=NEW(ExpNode);
        sematicGeneExp(root->subnode[1],subnode0);
        if(subnode0->error){
            node->error=1;
            //ERRORA
        }else{
            node->t=copyType(subnode0->t);
            node->error=0;
            destoryType(subnode0->t);
        }
        myFree(subnode0);
        node->left=subnode0->left;
        break;
    case 47:
        node->left=0;
        subnode0=NEW(ExpNode);
        sematicGeneExp(root->subnode[1],subnode0);
        if(subnode0->error){
            node->error=1;
            //ERRORA
        }else if(subnode0->t->type>2){
            node->error=1;
            printf("Error type 7 at line %d: Oprands type mismatched\n",root->line);
            printError();
            //ERRORB
        }else{
            node->t=copyType(subnode0->t);
            node->error=0;
            destoryType(subnode0->t);
        }
        myFree(subnode0);
        break;
    case 48:
        node->left=0;
        subnode0=NEW(ExpNode);
        sematicGeneExp(root->subnode[1],subnode0);
        if(subnode0->error){
            node->error=1;
            //ERRORA   
        }else if(subnode0->t->type!=INT){
            node->error=1;
            printf("Error type 7 at line %d: Oprands type mismatched\n",root->line);
            printError();
            //ERRORB
        }else{
            node->t=copyType(subnode0->t);
            node->error=0;
            destoryType(subnode0->t);
        }
        myFree(subnode0);
        break;
    case 49://ID LP Args RP
        node->left=0;
        leaf=root->subnode[0];
        funcname=(char*)myMalloc(sizeof(char)*(strlen(leaf->val.type_string)+1));
        strcpy(funcname,leaf->val.type_string);
        func=getFunction(funcname);
        if(func){
            subnode3=NEW(ArgsNode);
            subnode3->typeList=createList();
            sematicGeneArgs(root->subnode[2],subnode3);
            if(subnode3->error){
                node->error=1;
                //ERRORA
            }else{
                if(consistentArgs(subnode3->typeList,func->paraList)){
                    node->error=0;
                    node->t=copyType(func->retType);
                }else{
                    node->error=1;
                    printf("Error type 9 at line %d: The method \"%s\" is not appilcable for the arguments\n",root->line,funcname);
                    printError();
                    //ERRORB
                }
            }
            destoryList(subnode3->typeList);
            myFree(subnode3);
        }else{
            node->error=1;
            if(cnkiAll(funcname))
                printf("Error type 2 at line %d: Undefined function \"%s\"\n",root->line,funcname);
            else
                printf("Error type 11 at line %d: \"%s\" must be a function\n",root->line,funcname);
            printError();
            //ERRORB
        }
        myFree(funcname);

        break;
    case 50://ID LP RP
        node->left=0;
        leaf=root->subnode[0];
        func=getFunction(leaf->val.type_string);
        funcname=leaf->val.type_string;
        if(func){
            if(listIsEmpty(func->paraList)){
                node->error=0;
                node->t=copyType(func->retType);
            }else{
                node->error=1;
                printf("Error type 9 at line %d: The method \"%s\" is not appilcable for the arguments\n",root->line,funcname);
                printError();
                //ERRORB
            }
        }else{
            node->error=1;
            if(cnkiAll(funcname))
                printf("Error type 2 at line %d: Undefined function \"%s\"\n",root->line,funcname);
            else
                printf("Error type 11 at line %d: \"%s\" must be a function\n",root->line,funcname);
            printError();
            //ERRORB
        }
        break;
    case 51://EXP LB EXP RB
        node->left=1;
        subnode0=NEW(ExpNode);
        subnode1=NEW(ExpNode);
        sematicGeneExp(root->subnode[0],subnode0);
        sematicGeneExp(root->subnode[2],subnode1);
        if(subnode0->error||subnode1->error){
            node->error=1;
            //ERRORA
        }else if(subnode0->t->type!=4){
            node->error=1;
            printf("Error type 10 at line %d: Illegal use of \"[]\"\n",root->line);
            printError();
            //ERRORB
        }else if(subnode1->t->type!=INT){
            node->error=1;
            printf("Error type 12 at line %d: Array argument is not an integer\n",root->line);
            //ERRORB
        }else{
            node->t=copyType(subnode0->t->element);
            node->error=0;
        }
        if(!subnode0->error)
            destoryType(subnode0->t);
        if(!subnode1->error)
            destoryType(subnode1->t);
        myFree(subnode0);
        myFree(subnode1);
        break;
    case 52://Exp DOT ID
        node->left=1;
        leaf=root->subnode[2];
        subnode0=NEW(ExpNode);
        sematicGeneExp(root->subnode[0],subnode0);
        if(subnode0->error){
            node->error=1;
            //ERRORA
        }else{
            if(subnode0->t->type<5){
                //ERRORB
                node->error=1;
                printf("Error type 13 at line %d: Illegal use of \".\"\n",root->line);
                printError();
            }else{
                str=getStruct(subnode0->t->type);
                if(str==NULL){
                    //ERRORB
                    node->error=1;
                }else{
                    var=searchList(str->datalist,leaf->val.type_string,equalByNameStructField);
                    if(var==NULL){
                        //ERRORB
                        printf("Error type 14 at line %d: Un-existed feild \"%s\"\n",root->line,leaf->val.type_string);
                        node->error=1;
                    }else{
                        node->t=copyType(var->type);
                        node->error=0;
                    }
                }
            }
        }
        if(!subnode0->error)
            myFree(subnode0->t);
        myFree(subnode0);
        break;
    case 53://ID
        node->left=1;
        leaf=root->subnode[0];
        var=getVariable(leaf->val.type_string);
        if(var==NULL){
            node->error=1;
            printf("Error type 1 at line %d: Undefined variable \"%s\"\n",root->line,leaf->val.type_string);
            printError();
            //ERRORB
        }else{
            node->error=0;
            node->t=copyType(var->type);
        }
        break;
    case 54://INT
        node->left=0;
        node->error=0;
        node->t=NEW(AllType);
        node->t->type=INT;
        break;
    case 55://FLOAT
        node->left=0;
        node->error=0;
        node->t=NEW(AllType);
        node->t->type=FLOAT;
        break;
    default:
        printf("Error: invalid type");
        exit(1);
    }
    printEnd(root);
}

void sematicGeneArgs(struct TreeNode*root,void*attribute){
    printStart(root);
    ArgsNode*node=(ArgsNode*)attribute;
    if(root->type==56){//Exp COMMA Args
        ExpNode*subnode0=NEW(ExpNode);
        sematicGeneExp(root->subnode[0],subnode0);
        if(subnode0->error){
            node->error=1;
            //ERRORA
        }else{
            pushList(node->typeList,copyType(subnode0->t));
            if(node->error!=1)
                node->error=0;
        }
        destoryType(subnode0->t);
        myFree(subnode0);
        sematicGeneArgs(root->subnode[2],node);
    }else if(root->type==57){//Exp
        ExpNode*subnode0=NEW(ExpNode);
        sematicGeneExp(root->subnode[0],subnode0);
        if(subnode0->error){
            node->error=1;
            //ERRORA
        }else{
            pushList(node->typeList,copyType(subnode0->t));
            if(node->error!=1)
                node->error=0;
        }
        destoryType(subnode0->t);
        myFree(subnode0);
    }else{
        printf("Error: invalid type");
        exit(1);
    }
    printEnd(root);
}

StackEle*createStackEle(struct TreeNode*node,int level){
    StackEle*newStaEle=(StackEle*)myMalloc(sizeof(StackEle));
    newStaEle->node=node;
    newStaEle->level=level;
    return newStaEle;
}

Struct*createStruct(char*name){
    Struct*newStruct=(Struct*)myMalloc(sizeof(Struct));
    newStruct->name=(char*)myMalloc(sizeof(char)*(strlen(name)+1));
    strcpy(newStruct->name,name);
    newStruct->datalist=createList();
    return newStruct;
}

Struct*structAdd(Struct*str,Variable*src,int copy){
    Variable*newVariable=(Variable*)myMalloc(sizeof(Variable));
    newVariable->type=VARIABLE;
    if(copy){
        newVariable=copyVariable(src);
    }else{
        newVariable=src;
    }
    pushList(str->datalist,newVariable);
    return str;
}

Variable*copyVariable(Variable*var){
    Variable*newVariable=(Variable*)myMalloc(sizeof(Variable));
    newVariable->name=(char*)myMalloc(sizeof(char)*(strlen(var->name)+1));
    strcpy(newVariable->name,var->name);
    newVariable->type=copyType(var->type);
    return newVariable;
}

Struct*copyStruct(Struct*str){
    Struct*newStruct=(Struct*)myMalloc(sizeof(Struct));
    newStruct->name=(char*)myMalloc(sizeof(char)*(strlen(str->name)+1));
    strcpy(newStruct->name,str->name);
    newStruct->datalist=copyListPro(str->datalist,copyVariable);
    newStruct->num=str->num;
    return newStruct;
}

AllType*copyType(AllType*type){
    AllType* newType=NEW(AllType);
    newType->type=type->type;
    if(type->type!=4){//notarray
        newType->type=type->type;
    }else{
        newType->size=type->size;
        newType->element=copyType(type->element);
    }
    return newType;
}

Function*copyFunc(Function*func){
    Function*newFunc=NEW(Function);
    newFunc->name=(char*)myMalloc(sizeof(char)*(strlen(func->name)+1));
    strcpy(newFunc->name,func->name);
    newFunc->paraList=copyListPro(func->paraList,copyVariable);
    newFunc->retType=copyType(func->retType);
    return newFunc;
}

char*copyString(char*str){
    char*newStr=(char*)myMalloc(sizeof(char)*(strlen(str)+1));
    strcpy(newStr,str);
    return newStr;
}

void destoryStruct(Struct*str){
    if(str){
        myFree(str->name);
        destoryListPro(str->datalist,destoryVariable);
        myFree(str);
    }
}

void destoryType(AllType*type){
    if(type){
        if(type->type!=4){
            myFree(type);
        }else{
            destoryType(type->element);
            myFree(type);
        }
    }
}

void destoryVariable(Variable*var){
    if(var){
        myFree(var->name);
        destoryType(var->type);
        myFree(var);
    }
}

void destoryFunc(Function*func){
    if(func){
        myFree(func->name);
        destoryListPro(func->paraList,destoryVariable);
        destoryType(func->retType);
        myFree(func);
    }
}

int addStruct2Table(Struct*str){
    str->num=strNum;
    pushList(structTable,copyStruct(str));
    strNum++;
    return strNum-1;
}

void addVariable2Table(Variable*var){
    LinkedList*topVariableTable=topStack(variableTables);
    pushList(topVariableTable,copyVariable(var));
    IdVariable*idvar=NEW(IdVariable);
    varid++;
    idvar->id=varid;
    idvar->variable=copyVariable(var);
    pushList(translateTable,idvar);
}

void addVars2Table(LinkedList*var){
    if(var){
        LinkedList*node=var->next;
        while(node){
            addVariable2Table(node->data);
            node=node->next;
        }
    }
}

void addFunction2Table(Function*func){
    pushList(functionTable,copyFunc(func));
}

void deleteCompStFromTable(){
    LinkedList*top=topStack(variableTables);
    destoryListPro(top,destoryVariable);
    popStack(variableTables);
}

void addCompSt2Table(){
    LinkedList*newVariableTable=createList();
    pushStack(variableTables,newVariableTable);
}

int cnkiBlock(char*name){
    LinkedList*topVariableTable=topStack(variableTables);
    if(searchList(topVariableTable,name,equalByNameVar))
        return 0;
    if(searchList(structTable,name,equalByNameStruct))
        return 0;
    if(searchList(functionTable,name,equalByNameFunc))
        return 0;
    return 1;
}

int cnkiAll(char*name){
    LinkedList*variableTable=variableTables->next;
    while(variableTable){
        if(searchList(variableTable->data,name,equalByNameVar))
            return 0;
        variableTable=variableTable->next;
    }
    if(searchList(structTable,name,equalByNameStruct))
        return 0;
    if(searchList(functionTable,name,equalByNameFunc))
        return 0;
    return 1;
}

int equalByNameVar(void*var,void*name){
    return strcmp(((Variable*)var)->name,(char*)name)==0;
}

int equalByNameFunc(void*func,void*name){
    Function*function=(Function*)func;
    if(strcmp(function->name,(char*)name)==0){
        return 1;
    }else{
        return 0;
    }
}

int equalByNameStruct(void*str,void*name){
    Struct*struc=(Struct*)str;
    if(strcmp(struc->name,(char*)name)==0){
        return 1;
    }else{
        return 0;
    }
}

int equalByNameStructField(void*field,void*name){
    Variable*var=(Variable*)field;
    if(strcmp(var->name,(char*)name)==0){
        return 1;
    }else{
        return 0;
    }
}

Function* getFunction(char*name){
    void*funcTemp=searchList(functionTable,(void*)name,equalByNameFunc);
    if(funcTemp){
        Function*func=funcTemp;
        return func;
    }else{
        return NULL;
    }
}

Variable*getVariable(char*name){
    Stack*varTabNode=variableTables->next;
    LinkedList*variableTable=varTabNode->data;
    while(varTabNode){
        variableTable=varTabNode->data;
        Variable*var=searchList(variableTable,name,equalByNameVar);
        if(var)
            return var;
        varTabNode=varTabNode->next;
    }
}

Struct*getStructbName(char*name){
    void*strTemp=searchList(structTable,(void*)name,equalByNameStruct);
    if(strTemp){
        Struct*stru=strTemp;
        return stru;
    }else{
        return NULL;
    }
}

Struct*getStruct(int id){
    void*strtemp=listGet(structTable,id-5);
    if(strtemp){
        return strtemp;
    }else{
        return NULL;
    }
}

int consistentType(AllType*type0,AllType*type1){
    if(type0->type==type1->type&&type0->type!=4){
        return 1;
    }else if(type0->type==type1->type){
        if(type0->size==type1->size){
            return consistentType(type0->element,type1->element);
        }else{
            return 0; 
        }
    }else{
        return 0;
    }
}

int consistentArgs(LinkedList*list0,LinkedList*list1){
    LinkedList*node0=list0->next;
    LinkedList*node1=list1->next;
    while(node0&&node1){
        if(!consistentType(node0->data,((Variable*)node1->data)->type)){
            return 0;
        }
        node0=node0->next;
        node1=node1->next;
    }
    if(node0||node1){
        return 0;
    }else{
        return 1;
    }
}

void printError(){
}

long myMalloc(int len){
    nmalloc+=1;
    void*rt=malloc(len);
    memset(rt,0,len);
    // printf("M:%p\n",rt);
    pushListTest(mallocTable,rt);
    return rt;
}

int compareAddress(void*a,void*b){
    return a==b;
}

int printAddress(void*a){
    // printf("%p",a);
}

void myFree(void*a){
    nfree++;
    dropListElementTest(mallocTable,a,compareAddress);
    // printf("F:%p\n",a);
    free(a);
}

void printStart(struct TreeNode*root){
    // printf("%d:\n",root->type);
}

void printEnd(struct TreeNode*root){
    // printf(":%d\n",root->type);
}