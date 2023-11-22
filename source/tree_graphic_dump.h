#ifndef  __TREE_GRAPHIC_DUMP__
#define  __TREE_GRAPHIC_DUMP__

#include "tree_of_expressions.h"

int treeGraphicDump     (Tree *tree);
int writeTreeToDotFile  (Tree *tree, FILE *f);
char *createDumpFileName(int fileNumber);

int dotWriteNodes(Tree *tree, Node *node, FILE *f, int  rank);
int dotWriteEdges            (Node *node, FILE *f);

#endif //__TREE_GRAPHIC_DUMP__
