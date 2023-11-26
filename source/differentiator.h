#ifndef  __DIFFERENTIATOR_H__
#define  __DIFFERENTIATOR_H__

#include "tree_of_expressions.h"

Node *copy              (Evaluator *eval, Node *node);

Node *derivative        (Evaluator *eval, Node *node);
Node *processDifOperator(Evaluator *eval, Node *node);

#endif //__DIFFERENTIATOR_H__
