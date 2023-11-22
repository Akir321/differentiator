#ifndef  __EXP_TREE_IO_H__
#define  __EXP_TREE_IO_H__

int printNode      (Tree *tree, Node *node, FILE *f);
int printNodeSymbol(Tree *tree, Node *node, FILE *f);

int printTreeOperator      (ExpTreeOperators operatorType, FILE *f);
int printTreeOperatorSymbol(ExpTreeOperators operatorType, FILE *f);

int printTreePrefix (Tree *tree, Node *root, FILE *f);
int printTreeInfix  (Tree *tree, Node *root, FILE *f);
int printTreePostfix(Tree *tree, Node *root, FILE *f);

int expTreeNodePriority(Node *node);
bool isCommutative     (Node *node);

int printTreeInfixNoUselessBrackets(Tree *tree, Node *root, FILE *f);
int printNodeUsefulBrackets        (Tree *tree, Node *node, Node *parent, FILE *f);

#endif //__EXP_TREE_IO_H__
