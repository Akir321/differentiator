#ifndef  __EXP_TREE_READ_H__
#define  __EXP_TREE_READ_H__

#include "tree_of_expressions.h"

int readTree(Evaluator *eval, const char *fileName, Node * (*readNode)(Evaluator *, FILE *f));

int readTreePrefix(Evaluator *eval, const char *fileName);
int readTreeInfix (Evaluator *eval, const char *fileName);

Node *readNodePrefix(Evaluator *eval, FILE *f);
Node *readNodeInfix (Evaluator *eval, FILE *f);

int readNodeData(Evaluator *eval, ExpTreeNodeType *type, ExpTreeData *data, FILE *f, 
                 int (*processCommand)(Evaluator *, char *, ExpTreeData *, ExpTreeNodeType *));

int processStrExpTreeCommand      (Evaluator *eval, char *command, ExpTreeData *data, ExpTreeNodeType *type);
int processStrExpTreeCommandSymbol(Evaluator *eval, char *command, ExpTreeData *data, ExpTreeNodeType *type);

int addVariableToNameTable(Evaluator *eval, char *command, ExpTreeData *data, ExpTreeNodeType *type);

int skipSpaces(FILE *f);


#endif //__EXP_TREE_READ_H__
