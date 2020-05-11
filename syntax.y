%locations
%{
    #include<stdio.h>
    #include"tree.h"
    #include"lex.yy.c"
    int pass=1;
    int errorNum=0;
    extern int yylineno;
    extern void sematicAnalysis(struct TreeNode*root);
%}
/* declared types */
%union {
    struct TreeNode*node;
}

/* declared tokens */
%token <node> INT FLOAT TYPE ID
%token <node> STRUCT RETURN IF ELSE WHILE 
%token <node> SEMI COMMA ASSIGNOP RELOP
%token <node> PLUS MINUS STAR AND DIV OR DOT NOT
%token <node> LP RP LB RB LC RC

%right ASSIGNOP NOT
%left OR AND RELOP PLUS MINUS STAR DIV
%left LP RP LB RB DOT
%nonassoc COMMA LC RC TYPE STRUCT INT FLOAT RETURN IF WHILE ID
%nonassoc ELSE

%type <node> Program ExtDefList ExtDef ExtDecList Specifier
%type <node> StructSpecifier OptTag Tag VarDec FunDec VarList
%type <node> ParamDec CompSt StmtList Stmt DefList Def DecList
%type <node> Dec Exp Args

%%

Program:    ExtDefList  
                {
                    struct TreeNode*sub[1]={$1};
                    $$=newTreeNode(0,@$.first_line,1,sub);
                    if(pass){
                        traverseTree($$,0);
                        //sematicAnalysis($$);
                    }
                }
    ;
ExtDefList: ExtDef  ExtDefList  
                {
                    struct TreeNode*sub[2]={$1,$2};
                    $$=newTreeNode(1,@$.first_line,2,sub);
                }
    |           {
                    $$=newTreeNode(2,@$.first_line,0,0);
                }
    ;
ExtDef: Specifier ExtDecList SEMI   
                {
                    struct TreeNode*sub[3]={$1,$2,$3};
                    $$=newTreeNode(3,@$.first_line,3,sub);
                }
    | Specifier SEMI    
                {
                    struct TreeNode*sub[2]={$1,$2};
                    $$=newTreeNode(4,@$.first_line,2,sub);
                }
    | Specifier FunDec CompSt   
                {
                    struct TreeNode*sub[3]={$1,$2,$3};
                    $$=newTreeNode(5,@$.first_line,3,sub);
                }
    |   error SEMI {
                    //printf("error1");
        }
    ;
ExtDecList: VarDec  
                {
                    struct TreeNode*sub[1]={$1};
                    $$=newTreeNode(6,@$.first_line,1,sub);
                }
    |   VarDec COMMA ExtDecList 
                {
                    struct TreeNode*sub[3]={$1,$2,$3};
                    $$=newTreeNode(7,@$.first_line,3,sub);
                }
    ;

Specifier: TYPE 
                {
                    struct TreeNode*sub[1]={$1};
                    $$=newTreeNode(8,@$.first_line,1,sub);
                }
    |   StructSpecifier 
                {
                    struct TreeNode*sub[1]={$1};
                    $$=newTreeNode(9,@$.first_line,1,sub);
                }  
    ;
StructSpecifier:    STRUCT OptTag LC DefList RC 
                {
                    struct TreeNode*sub[5]={$1,$2,$3,$4,$5};
                    $$=newTreeNode(10,@$.first_line,5,sub);
                }
    |   STRUCT Tag  
                {
                    struct TreeNode*sub[2]={$1,$2};
                    $$=newTreeNode(11,@$.first_line,2,sub);
                }
    ;
OptTag: ID  
                {
                    struct TreeNode*sub[1]={$1};
                    $$=newTreeNode(12,@$.first_line,1,sub);
                }
    |           {
                    $$=newTreeNode(13,@$.first_line,0,0);
                }
    ;
Tag:    ID  
                {
                    struct TreeNode*sub[1]={$1};
                    $$=newTreeNode(14,@$.first_line,1,sub);
                }
    ;

VarDec: ID  
                {
                    struct TreeNode*sub[1]={$1};
                    $$=newTreeNode(15,@$.first_line,1,sub); 
                }
    |   VarDec LB INT RB    
                {
                    struct TreeNode*sub[4]={$1,$2,$3,$4};
                    $$=newTreeNode(16,@$.first_line,4,sub);
                }
    ;
FunDec: ID LP VarList RP    
                {
                    struct TreeNode*sub[4]={$1,$2,$3,$4};
                    $$=newTreeNode(17,@$.first_line,4,sub);
                }
    |   ID LP RP  
                {
                    struct TreeNode*sub[3]={$1,$2,$3};
                    $$=newTreeNode(18,@$.first_line,3,sub);
                }
    |  error RP {
                    //printf("error2");
                }
    ;
VarList:    ParamDec COMMA VarList   
                {
                    struct TreeNode*sub[3]={$1,$2,$3};
                    $$=newTreeNode(19,@$.first_line,3,sub);
                }
    |   ParamDec  
                {
                    struct TreeNode*sub[1]={$1};
                    $$=newTreeNode(20,@$.first_line,1,sub); 
                }
    ;
ParamDec:   Specifier VarDec    
                {
                    struct TreeNode*sub[2]={$1,$2};
                    $$=newTreeNode(21,@$.first_line,2,sub);
                }
    ;

CompSt: LC DefList StmtList RC  
                {
                    struct TreeNode*sub[4]={$1,$2,$3,$4};
                    $$=newTreeNode(22,@$.first_line,4,sub);
                }
   | LC error RC{ 
                    //printf("error3");
                }
    ;
StmtList:   Stmt StmtList   
                {
                    struct TreeNode*sub[2]={$1,$2};
                    $$=newTreeNode(23,@$.first_line,2,sub);
                }
    |           {
                    $$=newTreeNode(24,@$.first_line,0,0);
                }
    ;
Stmt:   Exp SEMI 
                {
                    struct TreeNode*sub[2]={$1,$2};
                    $$=newTreeNode(25,@$.first_line,2,sub);
                }
    |   CompSt  
                {
                    struct TreeNode*sub[1]={$1};
                    $$=newTreeNode(26,@$.first_line,1,sub); 
                }
    |   RETURN Exp SEMI 
                {
                    struct TreeNode*sub[3]={$1,$2,$3};
                    $$=newTreeNode(27,@$.first_line,3,sub);
                }
    |   IF LP Exp RP Stmt   
                {
                    struct TreeNode*sub[5]={$1,$2,$3,$4,$5};
                    $$=newTreeNode(28,@$.first_line,5,sub);
                }
    |   IF LP Exp RP Stmt ELSE Stmt    
                {
                    struct TreeNode*sub[7]={$1,$2,$3,$4,$5,$6,$7};
                    $$=newTreeNode(29,@$.first_line,7,sub);
                }
    |   WHILE LP Exp RP Stmt    
                {
                    struct TreeNode*sub[5]={$1,$2,$3,$4,$5};
                    $$=newTreeNode(30,@$.first_line,5,sub);
                }
    | error SEMI { 
                    //printf("error4");
                }
    ;

DefList:    Def DefList  
                {
                    struct TreeNode*sub[2]={$1,$2};
                    $$=newTreeNode(31,@$.first_line,2,sub);
                }
    |           {
                    $$=newTreeNode(32,@$.first_line,0,0);
                }
    ;
Def:    Specifier DecList SEMI  
                {
                    struct TreeNode*sub[3]={$1,$2,$3};
                    $$=newTreeNode(33,@$.first_line,3,sub);
                }
    |   error SEMI{ 
                    //printf("error5");
                 }
    ;
DecList:    Dec 
                {
                    struct TreeNode*sub[1]={$1};
                    $$=newTreeNode(34,@$.first_line,1,sub); 
                }
    |   Dec COMMA DecList   
                {
                    struct TreeNode*sub[3]={$1,$2,$3};
                    $$=newTreeNode(35,@$.first_line,3,sub);
                }
    ;
Dec:    VarDec  
                {
                    struct TreeNode*sub[1]={$1};
                    $$=newTreeNode(36,@$.first_line,1,sub); 
                }
    |   VarDec ASSIGNOP Exp
                {
                    struct TreeNode*sub[3]={$1,$2,$3};
                    $$=newTreeNode(37,@$.first_line,3,sub);
                }
    ;

Exp:    Exp ASSIGNOP Exp    
                {
                    struct TreeNode*sub[3]={$1,$2,$3};
                    $$=newTreeNode(38,@$.first_line,3,sub);
                }
    |   Exp AND Exp 
                {
                    struct TreeNode*sub[3]={$1,$2,$3};
                    $$=newTreeNode(39,@$.first_line,3,sub);
                }
    |   Exp OR Exp  
                {
                    struct TreeNode*sub[3]={$1,$2,$3};
                    $$=newTreeNode(40,@$.first_line,3,sub);
                }
    |   Exp RELOP Exp   
                {
                    struct TreeNode*sub[3]={$1,$2,$3};
                    $$=newTreeNode(41,@$.first_line,3,sub);
                }
    |   Exp PLUS Exp    
                {
                    struct TreeNode*sub[3]={$1,$2,$3};
                    $$=newTreeNode(42,@$.first_line,3,sub);
                }
    |   Exp MINUS Exp    
                {
                    struct TreeNode*sub[3]={$1,$2,$3};
                    $$=newTreeNode(43,@$.first_line,3,sub);
                }
    |   Exp STAR Exp    
                {
                    struct TreeNode*sub[3]={$1,$2,$3};
                    $$=newTreeNode(44,@$.first_line,3,sub);
                }
    |   Exp DIV Exp    
                {
                    struct TreeNode*sub[3]={$1,$2,$3};
                    $$=newTreeNode(45,@$.first_line,3,sub);
                }
    |   LP Exp RP    
                {
                    struct TreeNode*sub[3]={$1,$2,$3};
                    $$=newTreeNode(46,@$.first_line,3,sub);
                }
    |   MINUS Exp   
                {
                    struct TreeNode*sub[2]={$1,$2};
                    $$=newTreeNode(47,@$.first_line,2,sub);
                }
    |   NOT Exp 
                {
                    struct TreeNode*sub[2]={$1,$2};
                    $$=newTreeNode(48,@$.first_line,2,sub);
                }
    |   ID LP Args RP  
                {
                    struct TreeNode*sub[4]={$1,$2,$3,$4};
                    $$=newTreeNode(49,@$.first_line,4,sub);
                }
    |   ID LP RP  
                {
                    struct TreeNode*sub[3]={$1,$2,$3};
                    $$=newTreeNode(50,@$.first_line,3,sub);
                }
    |   Exp LB Exp RB   
                {
                    struct TreeNode*sub[4]={$1,$2,$3,$4};
                    $$=newTreeNode(51,@$.first_line,4,sub);
                }
    |   Exp DOT ID  
                {
                    struct TreeNode*sub[3]={$1,$2,$3};
                    $$=newTreeNode(52,@$.first_line,3,sub);
                }
    |   ID  
                {
                    struct TreeNode*sub[1]={$1};
                    $$=newTreeNode(53,@$.first_line,1,sub); 
                }
    |   INT 
                {
                    struct TreeNode*sub[1]={$1};
                    $$=newTreeNode(54,@$.first_line,1,sub); 
                }
    |   FLOAT   
                {
                    struct TreeNode*sub[1]={$1};
                    $$=newTreeNode(55,@$.first_line,1,sub); 
                }
    | Exp LB error RB{ 
                    //printf("error6");
                    } 
    ;
Args:   Exp COMMA Args  
                {
                    struct TreeNode*sub[3]={$1,$2,$3};
                    $$=newTreeNode(56,@$.first_line,3,sub);
                }
    |   Exp 
                {
                    struct TreeNode*sub[1]={$1};
                    $$=newTreeNode(57,@$.first_line,1,sub); 
                }
    ;
%%

int yyerror(char const*msg){
        printf("Error type B at Line %d: %s\n",yylineno,msg);
        pass=0;
}

int myerror(char const*msg,int yylineno){
    printf("Error type B at line %d: %s.\n",yylineno,msg);
}