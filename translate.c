#include"translate.h"
#include"semantic.h"
#include<stdio.h>

extern LinkedList*translateTable;
extern LinkedList*functionTable;
extern LinkedList*structTable;
extern Sentence*interCode;

int tnum=0;
int lnum=0;

Sentence*createSentence(){
    Sentence*newSentence=myMalloc(sizeof(Sentence));
    newSentence->last=newSentence->next=NULL;
    newSentence->str[0]=0;
    return newSentence;
}

char*newT(){
    tnum++;
    char*str=myMalloc(10*sizeof(char));
    str[0]='t';
    str[1]=0;
    char num[10];
    itoa(tnum,num,10);
    strcat(str,num);
    return str;
}

char*newL(){
    lnum++;
    char*str=myMalloc(10*sizeof(char));
    str[0]=0;
    strcat(str,"label");
    char num[10];
    itoa(lnum,num,10);
    strcat(str,num);
    return str;
}

int equalByNameIdVar(void*idvar,void*name){
    return strcmp(((IdVariable*)idvar)->variable->name,(char*)name)==0;
}

void destoryIdVariable(IdVariable*idvar){
    if(idvar){
        myFree(idvar->id);
        destoryVariable(idvar->variable);
        myFree(idvar);
    }
}

Sentence*codeAdd(Sentence*code1,Sentence*code2){
    //if(code1<1000||code2<1000)
    //    printf("%p,%p\n",code1,code2);
    if(code1&&code2){
        Sentence*p=code1;
        while(p->next)
            p=p->next;
        p->next=code2;
        code2->last=p;
    }else if(code2)
        code1=code2;
    return code1;
}

Sentence*translateExp(struct TreeNode*node,char*place,char*values,AllType*types,char*address){
    if(node->type==54){ //INT
        Sentence*code=NULL;
        if(place){
            code=createSentence();
            strcat(code->str,place);
            strcat(code->str," := #");
            char value[10];
            itoa(node->subnode[0]->val.type_int,value,10);
            strcat(code->str,value);
        }
        if(types){
            types->type=INT;
        }
        return code;
    }else if(node->type==53){ // ID
        IdVariable*idvar=searchList(translateTable,node->subnode[0]->val.type_string,equalByNameIdVar);
        char num[10];
        itoa(idvar->id,num,10);
        Sentence*code1=NULL;
        Sentence*code2=NULL;
        Sentence*code3=NULL;
        if(values){
            code1=createSentence();
            strcat(code1->str,"v");
            strcat(code1->str,num);
            strcat(code1->str," := ");
            strcat(code1->str,values);
        }
        if(place){
            code2=createSentence();
            strcat(code2->str,place);
            strcat(code2->str," := v");
            strcat(code2->str,num);
        }
        if(address){
            code3=createSentence();
            strcat(code3->str,address);
            strcat(code3->str," := &v");
            strcat(code3->str,num);
        }
        AllType*oldType=idvar->variable->type;
        if(types){
            if(oldType->type!=4){//notarray
                types->type=oldType->type;
            }else{
                types->size=oldType->size;
                types->type=oldType->type;
                types->element=copyType(oldType->element);
            }
        }

        code1=codeAdd(code1,codeAdd(code2,code3));
        return code1;
    }else if(node->type==38){   // Exp ASSIGNOP Exp
        char*t1=newT();
        AllType*tp=myMalloc(sizeof(AllType));
        Sentence*code1=translateExp(node->subnode[2],t1,NULL,tp,NULL);
        Sentence*code2=translateExp(node->subnode[0],place,t1,NULL,NULL);

        AllType*oldType=tp;
        if(types){
            if(oldType->type!=4){//notarray
                types->type=oldType->type;
            }else{
                types->type=oldType->type;
                types->size=oldType->size;
                types->element=copyType(oldType->element);
            }
        }
        
        code1=codeAdd(code1,code2);
        myFree(tp);
        myFree(t1);
        return code1;
    }else if(node->type==42){ // Exp PLUS Exp
        if(place){
            char*t1=newT();
            char*t2=newT();
            Sentence*code1=translateExp(node->subnode[0],t1,NULL,NULL,NULL);
            Sentence*code2=translateExp(node->subnode[2],t2,NULL,NULL,NULL);
            
            Sentence*code3=createSentence();
            strcat(code3->str,place);
            strcat(code3->str," := ");
            strcat(code3->str,t1);
            strcat(code3->str," + ");
            strcat(code3->str,t2);
            code1=codeAdd(code1,codeAdd(code2,code3));
            myFree(t1);
            myFree(t2);
            return code1;
        }else{
            return NULL;
        }
    }else if(node->type==43){ // Exp MINUS Exp
        if(place){
            char*t1=newT();
            char*t2=newT();
            Sentence*code1=translateExp(node->subnode[0],t1,NULL,NULL,NULL);
            Sentence*code2=translateExp(node->subnode[2],t2,NULL,NULL,NULL);
            
            Sentence*code3=createSentence();
            strcat(code3->str,place);
            strcat(code3->str," := ");
            strcat(code3->str,t1);
            strcat(code3->str," - ");
            strcat(code3->str,t2);
            code1=codeAdd(code1,codeAdd(code2,code3));
            myFree(t1);
            myFree(t2);
            return code1;
        }else{
            return NULL;
        }
    }else if(node->type==44){
         if(place){
            char*t1=newT();
            char*t2=newT();
            Sentence*code1=translateExp(node->subnode[0],t1,NULL,NULL,NULL);
            Sentence*code2=translateExp(node->subnode[2],t2,NULL,NULL,NULL);
            
            Sentence*code3=createSentence();
            strcat(code3->str,place);
            strcat(code3->str," := ");
            strcat(code3->str,t1);
            strcat(code3->str," * ");
            strcat(code3->str,t2);
            code1=codeAdd(code1,codeAdd(code2,code3));
            myFree(t1);
            myFree(t2);
            return code1;
        }else{
            return NULL;
        }       
    }else if(node->type==45){
        if(place){
            char*t1=newT();
            char*t2=newT();
            Sentence*code1=translateExp(node->subnode[0],t1,NULL,NULL,NULL);
            Sentence*code2=translateExp(node->subnode[2],t2,NULL,NULL,NULL);
            
            Sentence*code3=createSentence();
            strcat(code3->str,place);
            strcat(code3->str," := ");
            strcat(code3->str,t1);
            strcat(code3->str," / ");
            strcat(code3->str,t2);
            code1=codeAdd(code1,codeAdd(code2,code3));
            myFree(t1);
            myFree(t2);
            return code1;
        }else{
            return NULL;
        }
    }else if(node->type==47){
        if(place){
            char*t1=newT();
            Sentence*code1=translateExp(node->subnode[1],t1,NULL,NULL,NULL);
            Sentence*code2=createSentence();
            strcat(code2->str,place);
            strcat(code2->str," := #0 - ");
            strcat(code2->str,t1);
            code1=codeAdd(code1,code2);
            myFree(t1);
            return code1;
        }else{
            return NULL;
        }
    }else if(node->type==39||node->type==40||node->type==41||node->type==48){
        if(place){
            char*label1=newL();
            char*label2=newL();
            Sentence*code0=createSentence();
            strcat(code0,place);
            strcat(code0," := #0");

            Sentence*code1=translateCond(node,label1,label2);

            Sentence*code21=createSentence();
            strcat(code21->str,"LABEL ");
            strcat(code21->str,label1);
            strcat(code21->str," :");
            Sentence*code22=createSentence();
            strcat(code22->str,place);
            strcat(code22->str," := #1");

            Sentence*code3=createSentence();
            strcat(code3->str,"LABEL ");
            strcat(code3->str,label2);
            strcat(code3->str," :");

            code0=codeAdd(code0,codeAdd(code1,codeAdd(code21,codeAdd(code22,code3))));
            myFree(label1);
            myFree(label2);

            return code0;
        }else{
            return NULL;
        }
    }else if(node->type==50){ //ID LP RP
        Function*func=searchList(functionTable,node->subnode[0]->val.type_string,equalByNameFunc);
        if(strcmp(func->name,"read")==0){
            Sentence*code1=createSentence();
            strcat(code1->str,"READ ");
            strcat(code1->str,place);
            return code1;
        }else{
            Sentence*code2=createSentence();
            if(place){
                strcat(code2->str,place);
                strcat(code2->str," := CALL ");
            }else{
                char*tx=newT();
                strcat(code2->str,tx);
                strcat(code2->str," := CALL ");
            }
            strcat(code2->str,func->name);
            return code2;
        }
    }
    else if(node->type==49){    //ID LP Args RP
        Function*func=searchList(functionTable,node->subnode[0]->val.type_string,equalByNameFunc);
        LinkedList*arglist=createList();
        Sentence*code1=translateArgs(node->subnode[2],arglist);
        if(strcmp(func->name,"write")==0){
            Sentence*code2=createSentence();
            strcat(code2->str,"WRITE ");
            strcat(code2->str,listGet(arglist,0)); 
            Sentence*code3;
            if(place){
                code3=createSentence();
                strcat(code3->str,place);
                strcat(code3->str," := #0");
            }else{
               code3=NULL;
            }
            code1=codeAdd(code1,codeAdd(code2,code3));
        }else{
            LinkedList*p=arglist->next;
            LinkedList*param=func->paraList->next;
            while(p){

                // IdVariable*idvar=searchList(translateTable,p->data,equalByNameIdVar);
                // int type=idvar->variable->type->type;
                int type=((Variable*)(param->data))->type->type;
                Sentence*code4=createSentence();
                strcat(code4->str,"ARG ");
                if(type>3)
                    strcat(code4->str,"&");
                strcat(code4->str,p->data);
                code1=codeAdd(code1,code4);

                p=p->next;
                param=param->next;
            }
            Sentence*code5=createSentence();
            if(place){
                strcat(code5->str,place);
                strcat(code5->str," := CALL ");
            }else{
                char*tx=newT();
                strcat(code5->str,tx);
                strcat(code5->str," := CALL ");
            }
            strcat(code5->str,func->name);
            code1=codeAdd(code1,code5);
        }
        destoryListPro(arglist,myFree);
        return code1;
    }else if(node->type==51){   // Exp LB Exp RB
//        char*t0=newT();
        char*t1=newT();
        AllType*tp=myMalloc(sizeof(AllType));
        Sentence*code1=translateExp(node->subnode[0],NULL,NULL,tp,t1);

        char*t2=newT();
        Sentence*code2=translateExp(node->subnode[2],t2,NULL,NULL,NULL);
/*
        char*t1=newT();
        Sentence*code12=translateExpAddress(node->subnode[0],t1,t0,t2);
*/
        int tspace=spaceType(tp->element);
        char cspace[10];
        itoa(tspace,cspace,10);
        char*t3=newT();//shift
        Sentence*code3=createSentence();
        strcat(code3->str,t3);
        strcat(code3->str," := ");
        strcat(code3->str,t2);
        strcat(code3->str," * #");
        strcat(code3->str,cspace);
        
/*
        char*t3=newT();
        Sentence*code3=createSentence();
        strcat(code3->str,t3);// t3 = t2 * #4;
        strcat(code3->str," := ");
        strcat(code3->str,t2);
        strcat(code3->str," * ");
        strcat(code3->str,t22);
*/
        char*t4=newT();
        Sentence*code4=createSentence();// t4 = &t1 + t3
        strcat(code4->str,t4);
        strcat(code4->str," := ");
        strcat(code4->str,t1);
        strcat(code4->str," + ");
        strcat(code4->str,t3);

        Sentence*code5=NULL;
        if(values){
            code5=createSentence();
            strcat(code5->str,"*");
            strcat(code5->str,t4);
            strcat(code5->str," := ");
            strcat(code5->str,values);
        }

        Sentence*code6=NULL;
        if(place){
            code6=createSentence();
            strcat(code6->str,place);
            strcat(code6->str," := *");
            strcat(code6->str,t4);
        } 

        Sentence*code7=NULL;
        if(address){
            code7=createSentence();
            strcat(code7->str,address);
            strcat(code7->str," := ");
            strcat(code7->str,t1);           
            strcat(code7->str," + ");
            strcat(code7->str,t3);
        }

        AllType*oldType=tp->element;
        if(types){
            if(oldType->type!=4){//notarray
                types->type=oldType->type;
            }else{
                types->type=oldType->type;
                types->size=oldType->size;
                types->element=copyType(oldType->element);
            }
        }

        code1=codeAdd(code1,codeAdd(code2,codeAdd(code3,codeAdd(code4,codeAdd(code5,codeAdd(code6,code7))))));
        myFree(t1);
        myFree(t2);
        myFree(t3);
        myFree(tp);
        return code1;
        
    }else if(node->type==52){ // Exp DOT ID
//        char*t0=newT;
        char*t1=newT();
        AllType*tp=myMalloc(sizeof(AllType));
        Sentence*code1=translateExp(node->subnode[0],NULL,NULL,tp,t1);
/*
        char*t1=newT();
        Sentence*code12=translateExpAddress(node->subnode[0],t1,t0,NULL);

        code1=codeAdd(code1,code12);
*/
        Struct*stru=listGet(structTable,tp->type-5);
        char*name=copyString(node->subnode[2]->val.type_string);
        int shift=shiftStruct(stru,name);

        Variable*var=searchList(stru->datalist,name,equalByNameStructField);

        char*t2=newT();
        char value[10];
        itoa(shift,value,10);
            
        Sentence*code2=createSentence();
        strcat(code2->str,t2);
        strcat(code2->str," := ");
        strcat(code2->str,t1);
        strcat(code2->str," + #");
        strcat(code2->str,value);

        Sentence*code3=NULL;
        if(values){
            code3=createSentence();
            strcat(code3->str,"*");
            strcat(code3->str,t2);
            strcat(code3->str," := ");
            strcat(code3->str,values);
        }

        Sentence*code4=NULL;
        if(place){
            code4=createSentence();// t3 := *t3
            strcat(code4->str,place);
            strcat(code4->str," := *");
            strcat(code4->str,t2);
        }

        Sentence*code5=NULL;
        if(address){
            code5=createSentence();
            strcat(code5->str,address);
            strcat(code5->str," := ");
            strcat(code5->str,t1);           
            strcat(code5->str," + ");
            strcat(code5->str,value);
        }

        AllType*oldType=var->type;
        if(types){
            if(oldType->type!=4){//notarray
                types->type=oldType->type;
            }else{
                types->type=oldType->type;
                types->size=oldType->size;
                types->element=copyType(oldType->element);
            }
        }

        code1=codeAdd(code1,codeAdd(code2,codeAdd(code3,codeAdd(code4,code5))));
        myFree(t1);
        myFree(t2);
        myFree(tp);
        myFree(name);
        
        return code1;
    }else if(node->type==46){   // LP Exp RP
        Sentence*code1=translateExp(node->subnode[1],place,NULL,NULL,NULL);
        return code1;
    }else{
        printf("Translate Exp Error\n");
        return NULL;
    }
}
/*
Sentence*translateExpAddress(struct TreeNode*node,char*place,char*exp0,char*exp1){
    if(node->type==53){ // ID
        Sentence*code1=createSentence();
        strcat(code1->str,place);
        strcat(code1->str," := &v");
        
        IdVariable*idvar=searchList(translateTable,node->subnode[0]->val.type_string,equalByNameIdVar);
        char num[10];
        itoa(idvar->id,num,10);
        strcat(code1->str,num);
        return code1;
    }else if(node->type==51){ // Exp LB Exp RB
        Sentence*code1=createSentence();
        strcat(code1->str,place);
        strcat(code1->str," := &");
        strcat(code1->str,exp0);


        IdVariable*idvar=searchList(translateTable,node->subnode[0]->val.type_string,equalByNameIdVar);
        char num[10];
        itoa(idvar->id,num,10);
        strcat(code1->str,num);


        AllType*tp=myMalloc(sizeof(AllType));
        translateExp(node->subnode[0],NULL,NULL,tp,NULL);

        int ispace=spaceType(tp->element);
        char sspace[10];
        itoa(ispace,sspace,10);

        char*t1=newT();
        Sentence*code0=createSentence();
        strcat(code0->str,t1);
        strcat(code0->str," := ");
        strcat(code0->str,exp1);
        strcat(code0->str," * ");
        strcat(code0->str,sspace);

        strcat(code1->str," + ");
        strcat(code1->str,t1);

        code0=codeAdd(code0,code1);
        myFree(t1);

        return code0;
    }else if(node->type==52){ // Exp DOT ID
        Sentence*code1=createSentence();
        strcat(code1->str,place);
        strcat(code1->str," := &");
        strcat(code1->str,exp0);


        IdVariable*idvar=searchList(translateTable,node->subnode[0]->val.type_string,equalByNameIdVar);
        char num[10];
        itoa(idvar->id,num,10);
        strcat(code1->str,num);


        AllType*tp=myMalloc(sizeof(AllType));
        translateExp(node->subnode[0],NULL,NULL,tp,NULL);

        Struct*stru=listGet(structTable,tp->type-5);
        char*name=node->subnode[2]->val.type_string;
        int shift=shiftStruct(stru,name);

        char sshift[10];
        itoa(shift,sshift,10);

        strcat(code1->str," + ");
        strcat(code1->str,sshift);

        return code1;       
    }else{
        printf("translate Exp Address Error");
    }
}
*/
Sentence*translateStmt(struct TreeNode*node){
    if(node->type==25){
        return translateExp(node->subnode[0],NULL,NULL,NULL,NULL);
    }else if(node->type==26){
        return translateCompst(node->subnode[0]);
    }else if(node->type==27){
        char*t1=newT();
        Sentence*code1=translateExp(node->subnode[1],t1,NULL,NULL,NULL);
        Sentence*code2=createSentence();
        strcat(code2->str,"RETURN ");
        strcat(code2->str,t1);
        code1=codeAdd(code1,code2);
        myFree(t1);
        return code1;
    }else if(node->type==28){// IF LP Exp RP Stmt
        char*label1=newL();
        char*label2=newL();
        Sentence*code1=translateCond(node->subnode[2],label1,label2);
        Sentence*code2=translateStmt(node->subnode[4]);
        Sentence*code12=createSentence();
        strcat(code12->str,"LABEL ");
        strcat(code12->str,label1);
        strcat(code12->str," :");

        Sentence*code22=createSentence();
        strcat(code22->str,"LABEL ");
        strcat(code22->str,label2);
        strcat(code22->str," :");

        code1=codeAdd(code1,codeAdd(code12,codeAdd(code2,code22)));
        myFree(label1);
        myFree(label2); 
        return code1;
    }else if(node->type==29){ // IF LP Exp RP Stmt ELSE Stmt
        char*label1=newL();
        char*label2=newL();
        char*label3=newL();
        Sentence*code1=translateCond(node->subnode[2],label1,label2);
        Sentence*code2=translateStmt(node->subnode[4]);
        Sentence*code3=translateStmt(node->subnode[6]);
        Sentence*code12=createSentence();
        strcat(code12->str,"LABEL ");
        strcat(code12->str,label1);
        strcat(code12->str," :");

        Sentence*code22=createSentence();
        strcat(code22->str,"GOTO ");
        strcat(code22->str,label3);

        Sentence*code23=createSentence();
        strcat(code23->str,"LABEL ");
        strcat(code23->str,label2);
        strcat(code23->str," :");

        Sentence*code32=createSentence();
        strcat(code32->str,"LABEL ");
        strcat(code32->str,label3);
        strcat(code32->str," :");

        code1=codeAdd(code1,codeAdd(code12,codeAdd(code2,codeAdd(code22,codeAdd(code23,codeAdd(code3,code32))))));
        myFree(label1);
        myFree(label2);
        myFree(label3);
        return code1;
    }else if(node->type==30){   // WHILE LP Exp RP Stmt
        char*label1=newL();
        char*label2=newL();
        char*label3=newL();

        Sentence*code1=translateCond(node->subnode[2],label2,label3);
        Sentence*code2=translateStmt(node->subnode[4]);
        
        Sentence*code02=createSentence();
        strcat(code02->str,"LABEL ");
        strcat(code02->str,label1);
        strcat(code02->str," :");

        Sentence*code12=createSentence();
        strcat(code12->str,"LABEL ");
        strcat(code12->str,label2);
        strcat(code12->str," :");

        Sentence*code22=createSentence();
        strcat(code22->str,"GOTO ");
        strcat(code22->str,label1);

        Sentence*code23=createSentence();
        strcat(code23->str,"LABEL ");
        strcat(code23->str,label3);
        strcat(code23->str," :");

        code02=codeAdd(code02,codeAdd(code1,codeAdd(code12,codeAdd(code2,codeAdd(code22,code23)))));
        myFree(label1);
        myFree(label2);
        myFree(label3);
        return code02;
    }else{
        printf("Translate Stmt Error\n");
    }
}

Sentence*translateCond(struct TreeNode*node,char*ltrue,char*lfalse){
    if(node->type==41){
        char*t1=newT();
        char*t2=newT();
        Sentence*code1=translateExp(node->subnode[0],t1,NULL,NULL,NULL);
        Sentence*code2=translateExp(node->subnode[2],t2,NULL,NULL,NULL);
        char*op=node->subnode[1]->val.type_string;

        Sentence*code3=createSentence();
        strcat(code3->str,"IF ");
        strcat(code3->str,t1);
        strcat(code3->str," ");
        strcat(code3->str,op);
        strcat(code3->str," ");
        strcat(code3->str,t2);
        strcat(code3->str," GOTO ");
        strcat(code3->str,ltrue);
        
        Sentence*code4=createSentence();
        strcat(code4->str,"GOTO ");
        strcat(code4->str,lfalse);
        code1=codeAdd(code1,codeAdd(code2,codeAdd(code3,code4)));
        myFree(t1);
        myFree(t2);
        return code1;
    }else if(node->type==48){
        return translateCond(node->subnode[1],lfalse,ltrue);
    }else if(node->type==39){
        char*label1=newL();
        Sentence*code1=translateCond(node->subnode[0],label1,lfalse);
        Sentence*code2=translateCond(node->subnode[2],ltrue,lfalse);
        Sentence*code12=createSentence();
        strcat(code12->str,"LABEL ");
        strcat(code12->str,label1);
        strcat(code12->str," :");
        code1=codeAdd(code1,codeAdd(code12,code2));
        myFree(label1);
        return code1;
    }else if(node->type==40){
        char*label1=newL();
        Sentence*code1=translateCond(node->subnode[0],ltrue,label1);
        Sentence*code2=translateCond(node->subnode[2],ltrue,lfalse);
        Sentence*code12=createSentence();
        strcat(code12->str,"LABEL ");
        strcat(code12->str,label1);
        strcat(code12->str," :");
        code1=codeAdd(code1,codeAdd(code12,code2));
        myFree(label1);
        return code1;
    }else{
        char*t1=newT();
        Sentence*code1=translateExp(node,t1,NULL,NULL,NULL);
        Sentence*code2=createSentence();
        strcpy(code2->str,"IF ");
        strcpy(code2->str,t1);
        strcpy(code2->str," != #0 GOTO ");
        strcpy(code2->str,ltrue);
        Sentence*code3=createSentence();
        strcat(code3->str,"GOTO ");
        strcat(code3->str,lfalse);
        code1=codeAdd(code1,codeAdd(code2,code3));
        myFree(t1);
        return code1;
    }
}

Sentence*translateArgs(struct TreeNode*node,LinkedList*arglist){
    if(node->type==57){ //Exp
        char*t1=newT();
        Sentence*code1=translateExp(node->subnode[0],t1,NULL,NULL,NULL);
        pushList(arglist,copyString(t1));
        myFree(t1);
        return code1;
    }else if(node->type==56){   // Exp COMMA Args
        char*t1=newT();
        Sentence*code1=translateExp(node->subnode[0],t1,NULL,NULL,NULL);
        pushList(arglist,copyString(t1));
        Sentence*code2=translateArgs(node->subnode[2],arglist);
        code1=codeAdd(code1,code2);
        myFree(t1);
        return code1;
    }
}

Sentence*translateCompst(struct TreeNode*node){
    Sentence*code1=translateDefList(node->subnode[1]);
    Sentence*code2=translateStmtList(node->subnode[2]);
    return codeAdd(code1,code2);
}

Sentence*translateStmtList(struct TreeNode*node){
    if(node->type==23){
        Sentence*code1=translateStmt(node->subnode[0]);
        Sentence*code2=translateStmtList(node->subnode[1]);
        return codeAdd(code1,code2);
    }else if(node->type==24){
        return NULL;
    }
}

Sentence*translateExtDef(struct TreeNode*node){
    if(node->type==5){
        char*funcName=node->subnode[1]->subnode[0]->val.type_string;
        Sentence*code1=createSentence();
        strcat(code1,"FUNCTION ");
        strcat(code1,funcName);
        strcat(code1," :");

        Function*func=searchList(functionTable,funcName,equalByNameFunc);

        Sentence*code11=NULL;
        LinkedList*paramNode=func->paraList->next;
        while(paramNode){
            Sentence*code12=createSentence();
            strcat(code12->str,"PARAM v");
            char*varName=((Variable*)(paramNode->data))->name;
            IdVariable*idvar=searchList(translateTable,varName,equalByNameIdVar);
            char num[10];
            itoa(idvar->id,num,10);
            strcat(code12->str,num);
            code11=codeAdd(code12,code11);
            paramNode=paramNode->next;
        }
        code1=codeAdd(code1,code11);

        Sentence*code2=translateCompst(node->subnode[2]);
        code1=codeAdd(code1,code2);
        return code1;
    }else if(node->type==3||node->type==4){
        return NULL;
    }else{
        printf("translate extdef\n");
        return NULL;
    }
}

Sentence*translateExtDefList(struct TreeNode*node){
    if(node->type==1){
        Sentence*code1=translateExtDef(node->subnode[0]);
        Sentence*code2=translateExtDefList(node->subnode[1]);
        return codeAdd(code1,code2);
    }else if(node->type==2){
        return NULL;
    }
}

Sentence*translateProgram(struct TreeNode*node){
    if(node->type==0){
        Sentence*code1=translateExtDefList(node->subnode[0]);
        return code1;
    }
}

Sentence*translateDefList(struct TreeNode*node){
    if(node->type==31){ // Def DefList
        Sentence*code1=translateDef(node->subnode[0]);
        Sentence*code2=translateDefList(node->subnode[1]);
        return codeAdd(code1,code2);
    }else if(node->type==32){
        return NULL;
    }
};

Sentence*translateDef(struct TreeNode*node){
    if(node->type==33){ //Specifier DecList SEMI
        Sentence*code=translateDecList(node->subnode[1]);
        return code;
    }
}

Sentence*translateDecList(struct TreeNode*node){
    if(node->type==34){ //Dec
        return translateDec(node->subnode[0]);
    }else if(node->type==35){   //Dec COMMA DecList
        Sentence*code1=translateDec(node->subnode[0]);
        Sentence*code2=translateDecList(node->subnode[2]);
        return codeAdd(code1,code2);
    }
}

Sentence*translateDec(struct TreeNode*node){
    if(node->type==36){ //VarDec
        char name[20];
        Sentence*code=translateVarDec(node->subnode[0],name);
        return code;
    }else if(node->type==37){   //VarDec ASSIGNOP Exp
        char name[20];
        Sentence*code1=translateVarDec(node->subnode[0],name);
        IdVariable*idvar=searchList(translateTable,name,equalByNameIdVar);
        char place[20];
        strcpy(place,"v");
        char num[10];
        itoa(idvar->id,num,10);
        strcat(place,num);
        
        Sentence*code2=translateExp(node->subnode[2],place,NULL,NULL,NULL);
        return codeAdd(code1,code2);
    }
}

Sentence*translateVarDec(struct TreeNode*node,char*name){
    if(node->type==15){ //ID
        strcpy(name,node->subnode[0]->val.type_string);
        IdVariable*idvar=searchList(translateTable,name,equalByNameIdVar);
        if(idvar->variable->type->type<=2){
            return NULL;
        }else if(idvar->variable->type->type==4){
            Sentence*code1=createSentence();
            strcat(code1->str,"DEC v");
            char value1[10];
            itoa(idvar->id,value1,10);
            strcat(code1->str,value1);
            char value2[10];
            itoa(spaceArray(idvar->variable),value2,10);
            strcat(code1->str," ");
            strcat(code1->str,value2);
            return code1;
        }else if(idvar->variable->type->type>4){
            Sentence*code1=createSentence();
            strcat(code1->str,"DEC v");
            char value1[10];
            itoa(idvar->id,value1,10);
            strcat(code1->str,value1);
            char value2[10];
            int space=spaceStruct(idvar->variable);
            itoa(space,value2,10);
            strcat(code1->str," ");
            strcat(code1->str,value2);
            return code1;
        }else{
            printf("translate VarDec Error\n");
            return NULL;
        }
    }else if(node->type==16){   //VarDec LB INT RB
        return translateVarDec(node->subnode[0],name);
    }
}

int shiftStruct(Struct*stru,char*name){
    LinkedList*varList=stru->datalist->next;
    int swift=0;
    while(varList){
        if(equalByNameVar(varList->data,name)){
            return swift;       
        }else{
            if(((Variable*)(varList->data))->type->type<4){
                swift+=4;
            }else if(((Variable*)(varList->data))->type->type==4){
                swift+=spaceArray(varList->data);
            }else{
                swift+=spaceStruct(varList->data);
            }
        }
        varList=varList->next;
    }
    printf("Shift Struct Error");
    return 0;
}

int spaceStruct(Variable*var){
    if(var->type->type>4){
        Struct*stru=listGet(structTable,var->type->type-5);
        LinkedList*dataList=stru->datalist->next;
        int num=0;
        while(dataList){
            int type=((Variable*)(dataList->data))->type->type;
            if(type<4){
                num+=4;
            }else if(type==4){
                num+=spaceArray(dataList->data);
            }
            dataList=dataList->next;
        }
        return num;
    }else{
        printf("space Struct Error\n");
        return 4;
    }
}

void printCode(Sentence*code){
    while(code){
        printf("%s\n",code->str);
        code=code->next;
    }
}

int spaceArray(Variable*var){
    if(var->type->type==4)
        return spaceType(var->type->element)*var->type->size;
    else{
        printf("Error\n");
        return 4;
    }
}

int spaceStructType(struct at_*type){
    if(type->type>4){
        Struct*stru=listGet(structTable,type->type-5);
        LinkedList*dataList=stru->datalist->next;
        int num=0;
        while(dataList){
            int type=((Variable*)(dataList->data))->type->type;
            if(type<4){
                num+=4;
            }else if(type==4){
                num+=spaceArray(dataList->data);
            }
            dataList=dataList->next;
        }
        return num;
    }else{
        printf("space Struct Error\n");
        return 4;
    }
}

int spaceArrayType(struct at_*type){
    if(type->type==4)
        return spaceType(type->element)*type->size;
    else{
        printf("Error\n");
        return 4;
    }
}

int spaceType(struct at_*type){
    if(type->type<4){
        return 4;
    }else if(type->type==4){
        return spaceArrayType(type);
    }else{
        return spaceStructType(type);
    }
}

void printInterCode(FILE*fo){
    while(interCode){
        fprintf(fo,"%s\n",interCode->str);
        interCode=interCode->next;
    }
}

char* itoa(int num,char* str,int radix)
{/*索引表*/
    char index[]="0123456789ABCDEF";
    unsigned unum;/*中间变量*/
    int i=0,j,k;
    /*确定unum的值*/
    if(radix==10&&num<0)/*十进制负数*/
    {
        unum=(unsigned)-num;
        str[i++]='-';
    }
    else unum=(unsigned)num;/*其他情况*/
    /*转换*/
    do{
        str[i++]=index[unum%(unsigned)radix];
        unum/=radix;
       }while(unum);
    str[i]='\0';
    /*逆序*/
    if(str[0]=='-')
        k=1;/*十进制负数*/
    else
        k=0;
     
    for(j=k;j<=(i-1)/2;j++)
    {       char temp;
        temp=str[j];
        str[j]=str[i-1+k-j];
        str[i-1+k-j]=temp;
    }
    return str;
}