#ifndef  __DIFFERENTIATOR_H__
#define  __DIFFERENTIATOR_H__

#include "tree_of_expressions.h"

int differentiate       (Evaluator *from, Evaluator *to);

Node *copy              (Evaluator *eval, Node *node);

Node *derivative        (Evaluator *eval, Node *node);
Node *processDifOperator(Evaluator *eval, Node *node);

Node *difProcessLog (Evaluator *eval, Node *node);
Node *difRrocessPow (Evaluator *eval, Node *node);


int expTreeSimplify           (Evaluator *eval, Node *node);

int expTreeSimplifyConsts     (Evaluator *eval, Node *node);
int expTreeSimplifyNeutralElem(Evaluator *eval, Node *node);

int tryNodeSimplify(Evaluator *eval, Node *node);

int caseTimes0(Evaluator *eval, Node *node, Node *zero);
int casePlus0 (Evaluator *eval, Node *node, Node *zero, Node *savedNode);
int caseTimes1(Evaluator *eval, Node *node, Node *one,  Node *savedNode);
int casePow0  (Evaluator *eval, Node *node);
int casePow1  (Evaluator *eval, Node *node);

int  processArgv(int argC, const char *argV[], const char **fileInName, char **fileOutName);
void pointToZero(char *str);

#endif //__DIFFERENTIATOR_H__
