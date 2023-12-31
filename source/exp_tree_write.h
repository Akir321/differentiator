#ifndef  __EXP_TREE_IO_H__
#define  __EXP_TREE_IO_H__

int printNode      (Evaluator *eval, Node *node, FILE *f);
int printNodeSymbol(Evaluator *eval, Node *node, FILE *f);

int dumpNode       (Evaluator *eval, Node *node, FILE *f);

int printTreeVariable(Evaluator *eval, Node *node, FILE *f);

int printTreeOperator      (ExpTreeOperators operatorType, FILE *f);
int printTreeOperatorSymbol(ExpTreeOperators operatorType, FILE *f);

int printTreePrefix (Evaluator *eval, Node *root, FILE *f);
int printTreeInfix  (Evaluator *eval, Node *root, FILE *f);
int printTreePostfix(Evaluator *eval, Node *root, FILE *f);

int expTreeOperatorPriority(ExpTreeOperators oper);

int expTreeNodePriority(Node *node);
bool isCommutative     (Node *node);

int printTreeInfixNoUselessBrackets(Evaluator *eval, Node *root, FILE *f);
int printNodeUsefulBrackets        (Evaluator *eval, Node *node, Node *parent, FILE *f, 
                                    int (*printFunc)(Evaluator *, Node *, FILE *));

int printTreeToTexFile  (Evaluator *eval, Node *root, FILE *f, const char *prefix);
int printTreeTex        (Evaluator *eval, Node *root, FILE *f);
int printSubTreeTexStyle(Evaluator *eval, Node *root, FILE *f);

#endif //__EXP_TREE_IO_H__
