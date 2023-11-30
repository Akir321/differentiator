#ifndef  __PROG_DIFFERENTIATOR_H__
#define  __PROG_DIFFERENTIATOR_H__

#include "tree_of_expressions.h"

int runDifferentiator(Evaluator *function);

int firstDerivative (Evaluator *function, Evaluator *deriv1);
int secondDerivative(Evaluator *deriv1,   Evaluator *deriv2);

int taylor(Evaluator *function, Evaluator *deriv1, Evaluator *deriv2);

#endif //__PROG_DIFFERENTIATOR_H__
