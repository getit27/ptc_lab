#include<stdio.h>
#include"translate.h"
int main(int argc,char**argv){
    if(argc<=2)return 1;
    FILE*f=fopen(argv[1],"r");
    if(!f){
        perror(argv[1]);
        return 1;
    }
    yyrestart(f);
    yyparse();
    FILE*fo=fopen(argv[2],"w");
    if(!fo){
        perror(argv[2]);
        return 1;
    }
    printInterCode(fo);
    return 0;
}