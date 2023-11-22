#ifndef  __EXP_TREE_READ_H__
#define  __EXP_TREE_READ_H__

#include "tree_of_expressions.h"

Node *readTree(const char *fileName, Node * (*readNode)(FILE *f));

Node *readTreePrefix(const char *fileName);
Node *readTreeInfix (const char *fileName);

Node *readNodePrefix(FILE *f);
Node *readNodeInfix (FILE *f);

int readNodeData(ExpTreeNodeType *type, ExpTreeData *data, FILE *f, 
                 int (*processCommand)(char *, ExpTreeData *));

int processStrExpTreeCommand      (char *command, ExpTreeData *data);
int processStrExpTreeCommandSymbol(char *command, ExpTreeData *data);



#endif //__EXP_TREE_READ_H__
