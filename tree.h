#pragma once
typedef union Value{
    int type_int;
    float type_float;
    char* type_string
};

typedef struct TreeNode{
    int type;
    int leaf;
    union Value val;
    int line;
    int subnum;
    struct TreeNode*subnode[8]
};

struct TreeNode* newTreeNode(int type,int line,int subnum,struct TreeNode**subnode);

struct TreeNode* newTreeIntLeaf(int type,int line,int value);

struct TreeNode* newTreeFloatLeaf(int type,int line,float value);

struct TreeNode* newTreeStrLeaf(int type,int line,char* value);

void traverseTree(struct TreeNode*root,int level);

char*getTypeName(int code);

int cnkiBlock(char*name);//block struct field
int cnkiAll(char*name);//all struct field