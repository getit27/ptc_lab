#include"translate.h"
#include"semantic.h"
#include<stdio.h>

extern LinkedList*translateTable;
extern LinkedList*functionTable;
extern LinkedList*structTable;
int tnum=0;
int lnum=0;

Sentence*createSentence(){
    Sentence*newSentence=malloc(sizeof(Sentence));
    newSentence->last=newSentence->next=NULL;
    newSentence->str[0]=0;
    return newSentence;
}

char*newT(){
    tnum++;
    char*str=malloc(10*sizeof(char));
    str[0]='t';
    str[1]=0;
    char num[10];
    itoa(tnum,num,10);
    strcat(str,num);
    return str;
}

char*newL(){
    lnum++;
    char*str=malloc(10*sizeof(char));
    str[0]=0;
    strcat(str,"label");
    char num[10];
    itoa(lnum,num,10);
    strcat(str,num);
    strcat(str," :");
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
    if(code1&&code2){
        Sentence*p=code1;
        while(p->next)
            p=p->next;
        p->next=code2;
        code2->last=p;
        return code1;
    }else if(code1)
        return code1;
    else if(code2)
        return code2;
    else
        return NULL;
}

Sentence*translateExp(struct TreeNode*node,char*place){
    if(node->type==54){ //INT
        if(place){
            Sentence*newSentence=createSentence();
            strcat(newSentence->str,place);
            strcat(newSentence->str," := #");
            char value[10];
            itoa(node->val.type_int,value,10);
            strcat(newSentence->str,value);
            return newSentence;
        }else{
            return NULL;
        }
    }else if(node->type==53){ // ID
        if(place){
            Sentence*newSentence=createSentence();
            IdVariable*idvar=searchList(translateTable,node->subnode[0]->val.type_string,equalByNameIdVar);
            strcat(newSentence->str,place);
            strcat(newSentence->str," := v");
            char value[10];
            itoa(idvar->id,value,10);
            strcat(newSentence->str,value);
            return newSentence;
        }else{
            return NULL;
        }
    }else if(node->type==38){   // Exp ASSIGNOP Exp
        char*t1=newT();
        IdVariable*idvar=searchList(translateTable,node->subnode[0]->subnode[0]->val.type_string,equalByNameIdVar);
        Sentence*code1=translateExp(node->subnode[2],t1);
        char num[10];
        itoa(idvar->id,num,10);
        
        Sentence*code21=createSentence();
        strcat(code21->str,"v");
        strcat(code21->str,num);
        strcat(code21->str," := ");
        strcat(code21->str,t1);
        
        Sentence*code22;
        if(place){
            code22=createSentence();
            strcat(code22->str,place);
            strcat(code22->str," := v");
            strcat(code22->str,num);
        }else{
            code22=NULL;
        }

        codeAdd(code1,codeAdd(code21,code22));
        free(t1);
        return code1;
    }else if(node->type==42){
        if(place){
            char*t1=newT();
            char*t2=newT();
            Sentence*code1=translateExp(node->subnode[0],t1);
            Sentence*code2=translateExp(node->subnode[2],t2);
            
            Sentence*code3=createSentence();
            strcat(code3->str,place);
            strcat(code3->str," := ");
            strcat(code3->str,t1);
            strcat(code3->str," + ");
            strcat(code3->str,t2);
            codeAdd(code1,codeAdd(code2,code3));
            free(t1);
            free(t2);
            return code1;
        }else{
            return NULL;
        }
    }else if(node->type==47){
        if(place){
            char*t1=newT();
            Sentence*code1=translateExp(node->subnode[1],t1);
            Sentence*code2=createSentence();
            strcat(code2->str,place);
            strcat(code2->str," := #0 - ");
            strcat(code2->str,t1);
            codeAdd(code1,code2);
            free(t1);
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
            Sentence*code22=createSentence();
            strcat(code22->str,place);
            strcat(code22->str," := #1");

            Sentence*code3=createSentence();
            strcat(code3->str,"LABEL ");
            strcat(code3->str,label2);

            codeAdd(code0,codeAdd(code1,codeAdd(code21,codeAdd(code22,code3))));
            free(label1);
            free(label2);

            return code0;
        }else{
            return NULL;
        }
    }else if(node->type==50){ //ID LP RP
        Function*func=searchList(functionTable,node->subnode[0]->val.type_string,equalByNameFunc);
        if(strcmp(func->name,"read")==0){
            Sentence*code1=createList();
            strcat(code1->str,"READ ");
            strcat(code1->str,place);
            return code1;
        }else{
            Sentence*code2=createSentence();
            if(place){
                strcat(code2->str,place);
                strcat(code2->str," := CALL ");
            }else{
                strcat(code2->str,"CALL ");
            }
            strcat(code2,func->name);
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
            codeAdd(code1,codeAdd(code2,code3));
        }else{
            LinkedList*p=arglist->next;
            while(p){

                // IdVariable*idvar=searchList(translateTable,p->data,equalByNameIdVar);
                // int type=idvar->variable->type->type;
                int type=((Variable*)(p->data))->type->type;
                Sentence*code4=createSentence();
                strcat(code4->str,"ARG ");
                if(type>3)
                    strcat(code4->str,"&");
                strcat(code4->str,p->data);
                codeAdd(code1,code4);

                p=p->next;
            }
            Sentence*code5=createSentence();
            if(place){
                strcat(code5->str,place);
                strcat(code5->str," := CALL ");
            }else{
                strcat(code5->str,"CALL ");
            }
            codeAdd(code1,code5);
        }
        destoryListPro(arglist,free);
        return code1;
    }else if(node->type==51){   // Exp LB Exp RB
        if(place){
            char*t1=newT();
            Sentence*code1=translateExp(node->subnode[0],t1);
            char*t2=newT();
            Sentence*code2=translateExp(node->subnode[2],t2);
            char*t3=newT();
            Sentence*code3=createSentence();
            strcat(code3->str,t3);// t3 = t2 * 4;
            strcat(code3->str," := ");
            strcat(code3->str,t2);
            strcat(code3->str," * #4");

            Sentence*code4=createSentence();// t3 := t1 + t3
            strcat(code4->str,t3);
            strcat(code4->str," := &");
            strcat(code4->str,t1);
            strcat(code4->str," + ");
            strcat(code4->str,t3);

            Sentence*code5=createSentence();// t4 := *t3
            strcat(code5->str,place);
            strcat(code5->str," := *");
            strcat(code5->str,t3);
            
            codeAdd(code1,codeAdd(code2,codeAdd(code3,codeAdd(code4,code5))));
            free(t1);
            free(t2);
            free(t3);
            return code1;
        }
    }else if(node->type==52){ // Exp DOT ID
        if(place){
            IdVariable*idvar=searchList(translateTable,node->subnode[0]->val.type_string,equalByNameIdVar);
            Struct*stru=listGet(structTable,idvar->variable->type->type-4);
            char*name=copyString(node->subnode[2]->val.type_string);
            int shift=shiftStruct(stru,name);
            char*t1=newT();
            Sentence*code1=translateExp(node->subnode[0],t1);
            char*t2=newT();

            char value[10];
            itoa(shift,value,10);

            Sentence*code2=createSentence();
            strcat(code2->str,t2);
            strcat(code2->str," := &");
            strcat(code2->str,t1);
            strcat(code2->str," + #");
            strcat(code2->str,value);

            Sentence*code3=createSentence();// t3 := *t3
            strcat(code3->str,place);
            strcat(code3->str," := *");
            strcat(code3->str,t2);
            
            codeAdd(code1,code2);
            free(t1);
            free(t2);
            free(name);
            return code1;
        }
        return NULL;
    }else{
        printf("Error: Undfined Exp\n");
        return NULL;
    }
}

Sentence*translateStmt(struct TreeNode*node){
    if(node->type==25){
        return translateExp(node->subnode[0],NULL);
    }else if(node->type==26){
        return translateCompst(node->subnode[0]);
    }else if(node->type==27){
        char*t1=newT();
        Sentence*code1=translateExp(node->subnode[1],t1);
        Sentence*code2=createSentence();
        strcat(code2->str,"RETURN ");
        strcat(code2->str,t1);
        codeAdd(code1,code2);
        free(t1);
        return code1;
    }else if(node->type==28){
        char*label1=newL();
        char*label2=newL();
        Sentence*code1=translateCond(node->subnode[2],label1,label2);
        Sentence*code2=translateStmt(node->subnode[4]);
        codeAdd(code1,code2);
        free(label1);
        free(label2); 
        return code1;
    }else if(node->type==29){
        char*label1=newL();
        char*label2=newL();
        char*label3=newL();
        Sentence*code1=translateCond(node->subnode[2],label1,label2);
        Sentence*code2=translateStmt(node->subnode[4]);
        Sentence*code3=translateStmt(node->subnode[6]);
        Sentence*code12=createSentence();
        strcat(code12->str,"LABEL ");
        strcat(code12->str,label1);

        Sentence*code22=createSentence();
        strcat(code22->str,"GOTO ");
        strcat(code22->str,label3);

        Sentence*code23=createSentence();
        strcat(code23->str,"LABEL ");
        strcat(code23->str,label2);

        Sentence*code32=createSentence();
        strcat(code32->str,"LABEL ");
        strcat(code32->str,label3);

        codeAdd(code1,codeAdd(code12,codeAdd(code2,codeAdd(code22,codeAdd(code23,codeAdd(code3,code32))))));
        free(label1);
        free(label2);
        free(label3);
        return code1;
    }else if(node->type==30){
        char*label1=newL();
        char*label2=newL();
        char*label3=newL();

        Sentence*code1=translateCond(node->subnode[2],label2,label3);
        Sentence*code2=translateStmt(node->subnode[4]);
        
        Sentence*code02=createSentence();
        strcat(code02->str,"LABEL ");
        strcat(code02->str,label1);

        Sentence*code12=createSentence();
        strcat(code12->str,"LABEL ");
        strcat(code12->str,label2);

        Sentence*code22=createSentence();
        strcat(code22->str,"GOTO ");
        strcat(code22->str,label1);

        Sentence*code23=createSentence();
        strcat(code23->str,"LABEL ");
        strcat(code23->str,label3);

        codeAdd(code02,codeAdd(code1,codeAdd(code12,codeAdd(code12,codeAdd(code2,codeAdd(code22,code23))))));
        free(label1);
        free(label2);
        free(label3);
        return code1;
    }
}

Sentence*translateCond(struct TreeNode*node,char*ltrue,char*lfalse){
    if(node->type==41){
        char*t1=newT();
        char*t2=newT();
        Sentence*code1=translateExp(node->subnode[0],t1);
        Sentence*code2=translateExp(node->subnode[2],t2);
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
        codeAdd(code1,codeAdd(code2,codeAdd(code3,code4)));
        free(t1);
        free(t2);
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
        codeAdd(code1,codeAdd(code12,code2));
        free(label1);
        return code1;
    }else if(node->type==40){
        char*label1=newL();
        Sentence*code1=translateCond(node->subnode[0],ltrue,label1);
        Sentence*code2=translateCond(node->subnode[2],ltrue,lfalse);
        Sentence*code12=createSentence();
        strcat(code12->str,"LABEL ");
        strcat(code12->str,label1);
        codeAdd(code1,codeAdd(code12,code2));
        free(label1);
        return code1;
    }else{
        char*t1=newT();
        Sentence*code1=translateExp(node,t1);
        Sentence*code2=createSentence();
        strcpy(code2->str,"IF ");
        strcpy(code2->str,t1);
        strcpy(code2->str," != #0 GOTO ");
        strcpy(code2->str,ltrue);
        Sentence*code3=createSentence();
        strcat(code3->str,"GOTO ");
        strcat(code3->str,lfalse);
        codeAdd(code1,codeAdd(code2,code3));
        free(t1);
        return code1;
    }
}

Sentence*translateArgs(struct TreeNode*node,LinkedList*arglist){
    if(node->type==57){ //Exp
        char*t1=newT();
        Sentence*code1=translateExp(node->subnode[0],t1);
        pushList(arglist,copyString(t1));
        free(t1);
        return code1;
    }else if(node->type==56){   // Exp COMMA Args
        char*t1=newT();
        Sentence*code1=translateExp(node->subnode[0],t1);
        pushList(arglist,t1);
        Sentence*code2=translateArgs(node->subnode[2],arglist);
        codeAdd(code1,code2);
        free(t1);
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
        strcat(code1,":");
        Sentence*code2=translateCompst(node->subnode[2]);
        codeAdd(code1,code2);
        return code1;
    }
}

Sentence*translateExtDefList(struct TreeNode*node){
    if(node->type==1){
        return codeAdd(translateExtDef(node->subnode[0]),translateExtDefList(node->subnode[1]));
    }else if(node->type==2){
        return NULL;
    }
}

Sentence*translateProgram(struct TreeNode*node){
    if(node->type==0){
        return translateExtDefList(node->subnode[0]);
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
        return translateDecList(node->subnode[1]);
    }
}

Sentence*translateDecList(struct TreeNode*node){
    if(node->type==34){ //Dec
        return translateDec(node->subnode[0]);
    }else if(node->type==35){   //Dec COMMA DecList
        return codeAdd(translateDec(node->subnode[0]),translateDec(node->subnode[2]));
    }
}

Sentence*translateDec(struct TreeNode*node){
    if(node->type==36){ //VarDec
        char*name;
        return translateVarDec(node->subnode[0],name);
    }else if(node->type==37){   //VarDec ASSIGNOP Exp
        char*name;
        return codeAdd(translateVarDec(node->subnode[0],name),translateExp(node->subnode[1],name));
    }
}

Sentence*translateVarDec(struct TreeNode*node,char*name){
    if(node->type==15){ //ID
        name=copyString(node->subnode[0]->val.type_string);
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
            itoa(spaceStruct(idvar->variable),value2,10);
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
    LinkedList*varList=stru->datalist;
    int swift=0;
    while(varList){
        if(equalByNameVar(varList->data,name)){
            return swift;       
        }else{
            if(((Variable*)(varList->data))->type<4){
                swift+=4;
            }else if(((Variable*)(varList->data))->type==4){
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
        Struct*stru=listGet(structTable,var->type->type-4);
        LinkedList*dataList=stru->datalist;
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
        return 4;
    }else{
        printf("Error\n");
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
        return 4*var->type->size;
    else{
        printf("Error\n");
        return 4;
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