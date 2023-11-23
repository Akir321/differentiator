#ifndef  __TREE_GRAPHIC_DUMP__
#define  __TREE_GRAPHIC_DUMP__

#include "tree_of_expressions.h"

int treeGraphicDump     (Evaluator *eval);
int writeTreeToDotFile  (Evaluator *eval, FILE *f);
char *createDumpFileName(int fileNumber);

int dotWriteNodes(Evaluator *eval, Node *node, FILE *f, int  rank);
int dotWriteEdges                 (Node *node, FILE *f);

#endif //__TREE_GRAPHIC_DUMP__
