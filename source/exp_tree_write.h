#ifndef  __EXP_TREE_IO_H__
#define  __EXP_TREE_IO_H__

int printNode      (Node *node, FILE *f);
int printNodeSymbol(Node *node, FILE *f);

int printTreeOperator      (ExpTreeOperators operatorType, FILE *f);
int printTreeOperatorSymbol(ExpTreeOperators operatorType, FILE *f);

int printTreePrefix (Node *root, FILE *f);
int printTreeInfix  (Node *root, FILE *f);
int printTreePostfix(Node *root, FILE *f);

int expTreeNodePriority(Node *node);
bool isCommutative     (Node *node);

int printTreeInfixNoUselessBrackets(Node *root, FILE *f);
int printNodeUsefulBrackets        (Node *node, Node *parent, FILE *f);

#endif //__EXP_TREE_IO_H__
