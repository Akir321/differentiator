#ifndef  __EXP_TREE_READ_H__
#define  __EXP_TREE_READ_H__

#include "tree_of_expressions.h"

Node *readTreePrefix(const char *fileName);

Node *readNodePrefix(FILE *f);

int readNodeData(ExpTreeNodeType *type, ExpTreeData *data, FILE *f);

int processStrExpTreeCommand(char *command, ExpTreeData *data);


#endif //__EXP_TREE_READ_H__
