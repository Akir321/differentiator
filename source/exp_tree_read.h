#ifndef  __EXP_TREE_READ_H__
#define  __EXP_TREE_READ_H__

#include "tree_of_expressions.h"

int readTree(Tree *tree, const char *fileName, Node * (*readNode)(Tree *, FILE *f));

int readTreePrefix(Tree *tree, const char *fileName);
int readTreeInfix (Tree *tree, const char *fileName);

Node *readNodePrefix(Tree *tree, FILE *f);
Node *readNodeInfix (Tree *tree, FILE *f);

int readNodeData(Tree *tree, ExpTreeNodeType *type, ExpTreeData *data, FILE *f, 
                 int (*processCommand)(Tree *, char *, ExpTreeData *, ExpTreeNodeType *));

int processStrExpTreeCommand      (Tree *tree, char *command, ExpTreeData *data, ExpTreeNodeType *type);
int processStrExpTreeCommandSymbol(Tree *tree, char *command, ExpTreeData *data, ExpTreeNodeType *type);

int addVariableToNameTable(Tree *tree, char *command, ExpTreeData *data, ExpTreeNodeType *type);


#endif //__EXP_TREE_READ_H__
