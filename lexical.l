%{
    #include"syntax.tab.h"
    #include"tree.h"
    int yycolumn = 1;
    #define YY_USER_ACTION \
        yylloc.first_line = yylloc.last_line = yylineno; \
        yylloc.first_column = yycolumn; \
        yylloc.last_column = yycolumn + yyleng - 1; \
        yycolumn += yyleng;
    extern int pass;
    extern YYSTYPE valueInt(int i);
    extern YYSTYPE valueFloat(double d);
    extern YYSTYPE valueString(char* s);
%}

%option yylineno

digit	[0-9]
letter	[_a-zA-Z]
delim	[ \n\t\r]
ws	{delim}+
id	{letter}({letter}|{digit})*
number	{digit}+
float	{number}?\.{number}([Ee][+-]?{number})?

%%
int|float   {yylval.node = newTreeStrLeaf(TYPE,yylineno,yytext);         return(TYPE);}
struct		{yylval.node = newTreeIntLeaf(STRUCT,yylineno,atoi(yytext)); return STRUCT;}
return		{yylval.node = newTreeIntLeaf(RETURN,yylineno,atoi(yytext)); return RETURN;}
if		    {yylval.node = newTreeIntLeaf(IF,yylineno,atoi(yytext));     return IF;}
else		{yylval.node = newTreeIntLeaf(ELSE,yylineno,atoi(yytext));   return ELSE;}
while		{yylval.node = newTreeIntLeaf(WHILE,yylineno,atoi(yytext));  return WHILE;}
\n          { yycolumn = 1; }
{ws}        {/* no action and no return*/}
{float}		{yylval.node = newTreeFloatLeaf(FLOAT,yylineno,atof(yytext));return(FLOAT);}
{number}	{yylval.node = newTreeIntLeaf(INT,yylineno,atoi(yytext));    return(INT);}
{id}		{yylval.node = newTreeStrLeaf(ID,yylineno,yytext);           return(ID);}
";"		    {yylval.node = newTreeIntLeaf(SEMI,yylineno,atoi(yytext));   return (SEMI);}
","		    {yylval.node = newTreeIntLeaf(COMMA,yylineno,atoi(yytext));  return (COMMA);}
"="		    {yylval.node = newTreeIntLeaf(ASSIGNOP,yylineno,atoi(yytext));return (ASSIGNOP);}
"<"         {yylval.node = newTreeStrLeaf(RELOP,yylineno,yytext);        return(RELOP);}
">"         {yylval.node = newTreeStrLeaf(RELOP,yylineno,yytext);        return(RELOP);}
"<="        {yylval.node = newTreeStrLeaf(RELOP,yylineno,yytext);        return(RELOP);}
">="        {yylval.node = newTreeStrLeaf(RELOP,yylineno,yytext);        return(RELOP);}
"=="        {yylval.node = newTreeStrLeaf(RELOP,yylineno,yytext);        return(RELOP);}
"!="        {yylval.node = newTreeStrLeaf(RELOP,yylineno,yytext);        return(RELOP);}
\+		    {yylval.node = newTreeIntLeaf(PLUS,yylineno,atoi(yytext));   return PLUS;}
\-		    {yylval.node = newTreeIntLeaf(MINUS,yylineno,atoi(yytext));  return MINUS;}
\*		    {yylval.node = newTreeIntLeaf(STAR,yylineno,atoi(yytext));   return STAR;}
&&		    {yylval.node = newTreeIntLeaf(AND,yylineno,atoi(yytext));    return AND;}
\/          {yylval.node = newTreeIntLeaf(DIV,yylineno,atoi(yytext));    return DIV;}
\|\|		{yylval.node = newTreeIntLeaf(OR,yylineno,atoi(yytext));     return OR;}
\.		    {yylval.node = newTreeIntLeaf(DOT,yylineno,atoi(yytext));    return DOT;}
!		    {yylval.node = newTreeIntLeaf(NOT,yylineno,atoi(yytext));    return NOT;}
\(		    {yylval.node = newTreeIntLeaf(LP,yylineno,atoi(yytext));     return LP;}
\)		    {yylval.node = newTreeIntLeaf(RP,yylineno,atoi(yytext));     return RP;}
\[		    {yylval.node = newTreeIntLeaf(LB,yylineno,atoi(yytext));     return LB;}
\]		    {yylval.node = newTreeIntLeaf(RB,yylineno,atoi(yytext));     return RB;}
\{		    {yylval.node = newTreeIntLeaf(LC,yylineno,atoi(yytext));     return LC;}
\}		    {yylval.node = newTreeIntLeaf(RC,yylineno,atoi(yytext));     return RC;}
.           {printf("Error type A at Line %d: Mysterious character \"%s\".\n",yylineno,yytext);pass=0;}

%%

union YYSTYPE TreeIntLeaf(int type,int line,int value){
    YYSTYPE y;
    y.node=newTreeIntLeaf(type,line,value);
    return y;
}
union YYSTYPE TreeFloatLeaf(int type,int line,float value){
    YYSTYPE y;
    y.node=newTreeFloatLeaf(type,line,value);
    return y;
}
union YYSTYPE TreeStrLeaf(int type,int line,char*value){
    YYSTYPE y;
    y.node=newTreeStrLeaf(type,line,value);
    return y;
}